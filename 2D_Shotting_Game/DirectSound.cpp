
LPDIRECTSOUND lpds;
DSBUFFERDESC dsbd;

typedef struct pcm_sound_type
{
	LPDIRECTSOUNDBUFFER dsbuffer;
	int state;
	int rate;
	int size;
	int id;
}pcm_sound, *pcm_sound_ptr;
pcm_sound sound_fx[MAX_SOUNDS];

int DirectSound_Init(HWND window)
{
	//Creating Sound Device
	if (DirectSoundCreate(NULL, &lpds, NULL) != DS_OK)
	{
		MessageBox(window, "Error Creating Sound!", "Error", MB_OK);
		return 0;
	}
	if (FAILED(lpds->SetCooperativeLevel(window, DSSCL_NORMAL)))
	{
		MessageBox(window, "Error setting CooperativeLevel", "Error", MB_OK);
		return 0;
	}
	//Initializing All Sounds
	memset(sound_fx, 0, sizeof(pcm_sound)*MAX_SOUNDS);
	for (int i = 0; i < MAX_SOUNDS; ++i)
	{
		if (sound_fx[i].dsbuffer)
		{
			sound_fx[i].dsbuffer->Stop();
			sound_fx[i].dsbuffer->Release();
		}
		memset(&sound_fx[i], 0, sizeof(pcm_sound));
		sound_fx[i].state = SOUND_NULL;
		sound_fx[i].id = i;
	}
	int id = DSound_Load_WAV(window, (LPWSTR)"wind.wav");
	if (id == -1)
		return 0;
	sound_fx[id].dsbuffer->Play(0, 0, DSBPLAY_LOOPING);
}

void DirectSound_Shutdown()
{
	for (DWORD i = 0; i < MAX_SOUNDS; ++i)
	{
		if (sound_fx[i].dsbuffer)
			sound_fx[i].dsbuffer->Release();
	}
	if (lpds)
		lpds->Release();
}

int DSound_Load_WAV(HWND window, LPWSTR filename, bool is_default)
{
	HMMIO hwav;
	MMCKINFO parent;
	MMCKINFO child;
	WAVEFORMATEX wfmtx;
	int sound_id = -1;
	int index;
	UCHAR* snd_buffer;
	UCHAR* audio_ptr_1 = NULL;
	UCHAR* audio_ptr_2 = NULL;
	DWORD audio_length_1 = 0;
	DWORD audio_length_2 = 0;
	for (index = 0; index < MAX_SOUNDS; ++index)
	{
		if (sound_fx[index].state == SOUND_NULL)
		{
			sound_id = index;
			break;
		}
	}
	if ((sound_id == -1))
	{
		MessageBox(window, "Memory is used!", "Error", MB_OK);
		return -1;
	}
	//initializing the informatino block
	parent.ckid = (FOURCC)0;
	parent.cksize = 0;
	parent.fccType = (FOURCC)0;
	parent.dwDataOffset = 0;
	parent.dwFlags = 0;
	child = parent;

	//open file into hwav
	if ((hwav = mmioOpen((LPSTR)filename, NULL, MMIO_READ | MMIO_ALLOCBUF)) == NULL)
	{
		MessageBox(window, "Fail to open WAV file!", "Error", MB_OK);
		return -1;
	}

	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if (mmioDescend(hwav, &parent, NULL, MMIO_FINDRIFF))
	{
		mmioClose(hwav, 0);
		MessageBox(window, "Failed to move WAV_ptr", "Error", MB_OK);
		return -1;
	}

	child.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hwav ,&child, &parent, 0))
	{
		mmioClose(hwav, 0);
		MessageBox(window, "Failed to move WAV FMT", "Error", MB_OK);
		return -1;
	}

	if (mmioRead(hwav, (char*)&wfmtx, sizeof(wfmtx)) != sizeof(wfmtx))
	{
		mmioClose(hwav, 0);
		MessageBox(window, "Failed to Read WAV", "Error", MB_OK);
		return -1;
	}
	if (wfmtx.wFormatTag != WAVE_FORMAT_PCM)
	{
		mmioClose(hwav, 0);
		MessageBox(window, "WAV info is not PCM", "Error", MB_OK);
		return -1;
	}
	if (mmioAscend(hwav, &child, 0))
	{
		mmioClose(hwav, 0);
		MessageBox(window, "WAV to end failed", "Error", MB_OK);
		return -1;
	}
	child.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hwav, &child, &parent, MMIO_FINDCHUNK))
	{
		mmioClose(hwav, 0);
		MessageBox(window, "WAV to data failed", "Error", MB_OK);
		return -1;
	}
	snd_buffer = (UCHAR*)malloc(child.cksize);
	mmioRead(hwav, (char*)snd_buffer, child.cksize);
	mmioClose(hwav, 0);

	sound_fx[sound_id].rate = wfmtx.nSamplesPerSec;
	sound_fx[sound_id].size = child.cksize;
	sound_fx[sound_id].state = SOUND_LOADED;

	WAVEFORMATEX pcmwf;
	memset(&pcmwf, 0, sizeof(WAVEFORMATEX));
	pcmwf.wFormatTag = WAVE_FORMAT_PCM;
	pcmwf.nChannels = 1;
	pcmwf.nSamplesPerSec = 11025;
	pcmwf.nBlockAlign = 1;
	pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
	pcmwf.wBitsPerSample = 8;
	pcmwf.cbSize = 0;

	dsbd.dwSize = sizeof(DSBUFFERDESC);
	if (is_default)
		dsbd.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
	dsbd.dwBufferBytes = child.cksize;
	dsbd.lpwfxFormat = &pcmwf;

	if (lpds->CreateSoundBuffer(&dsbd, &sound_fx[sound_id].dsbuffer, NULL) != DS_OK)
	{
		free(snd_buffer);
		MessageBox(window, "Faile Creating Sound Cache!", "Error", MB_OK);
		return -1;
	}

	if (sound_fx[sound_id].dsbuffer->Lock(0, child.cksize, (void**)&audio_ptr_1, &audio_length_1, (void**)&audio_ptr_2, &audio_length_2, DSBLOCK_FROMWRITECURSOR) != DS_OK)
	{
		MessageBox(window, "Faile add lock", "Error", MB_OK);
		return 0;
	}
	memcpy(audio_ptr_1, snd_buffer, audio_length_1);
	memcpy(audio_ptr_2, snd_buffer + audio_length_1, audio_length_2);

	if (sound_fx[sound_id].dsbuffer->Unlock(audio_ptr_1, audio_length_1, audio_ptr_2, audio_length_2) != DS_OK)
	{
		MessageBox(window, "Error Unlock!", "Error", MB_OK);
		return 0;
	}
	free(snd_buffer);

	return sound_id;
}

