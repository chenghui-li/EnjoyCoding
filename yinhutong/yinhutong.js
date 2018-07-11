const express = require('express');
var http = require('http');
//var request = require('request');
const app = express();
/*
vim中文乱码问题：
vim /etc/vimrc
加入以下内容：
set fileencodings=utf-8,ucs-bom,gb18030,gbk,gb2312,cp936
   set termencoding=utf-8
   set encoding=utf-8
*/

/***********************************************************************************************************************************/
/***********************************************************************************************************************************/


/*
 * @function 服务首页,第一行由前端配置文件完成，第二行从后台获取
 */
var serverHome = function(){
    /*
    [[建设银行,111],[工商银行,222].[中国银行,333],[招商银行,444]]

    */
    var bankList = [['建设银行',111],['工商银行',222],['中国银行',333],['招商银行',444]];
    var serviceNumber = bankList.length;
    var list = '[';
    for(var i = 0;i<serviceNumber;i++){
        var item = '{';
        item = item + '"name": "'+ bankList[i][0] + '",';//服务名称
        item = item + '"logo": "'+ bankList[i][1] + '"}';//服务logo
        if(i != serviceNumber-1)
            item += ',';
        list += item;
    }
    list += ']';
    return JSON.parse(list);
}

/* @function 服务-选择银行
 * @parm {string} service_id 服务id
 * @return {JSON数组} 支持该服务的银行
 */
var selectBank = function(service_id){
    var bankList = [['建设银行',111,'121'],['工商银行',222,'232'],['中国银行',333,'343'],['招商银行',444,'454']];  //二维数组
    var bankNumber = bankList.length;
    var list = '[';
    for(var i = 0;i<bankNumber;i++){
	var item = '{';
	item = item + '"name": "'+ bankList[i][0] + '",';//银行名称
	item = item + '"logo": "'+ bankList[i][1] + '",';//银行logo
	item = item + '"url": "' + bankList[i][2] + '"}';//银行服务访问地址
	if(i != bankNumber -1)
	    item += ',';
	list += item;
    }
    list += ']';
    return JSON.parse(list);
}

/*
 * @function 首页中显示银行的部分
 */
var showBankInfo = function(){
    var bankInfo = [['建设银行',111,'121'],['工商银行',222,'232'],['中国银行',333,'343'],['招商银行',444,'454']];    //从后台获取所有接入的银行列表，返回二维数组
    var bankNumbers = bankInfo.length;//the number of bank
    var infoNumbers = bankInfo[0].length;//the number of item
    //解析为json对象返回给前端
    var list = '[';
    for(var i = 0;i<bankNumbers;i++){
        var item = '{';
        /*for(var j = 0;j<infoNumbers;j++){
            item = item + '"'+bankInfo[i][j];
            item += ',';
        }*/
        item = item + '"name": "' + bankInfo[i][0] + '",';//银行名称
        item = item + '"logo": "' + bankInfo[i][1] + '",';//logo
        item = item + '"url": "' + bankInfo[i][2] + '"}';//银行服务访问地址
        if(i != bankNumbers-1)
            item += ',';
        list += item;
    }
    list += ']';
    return JSON.parse(list);
}
/*
 * @function 服务全部列表页面
 */
var allService = function(){
    var serviceList = [['猜你喜欢','取款','111'],['猜你喜欢','存款','222'],['基本服务','理财','333']];  //二维数组
    var serviceNumber = serviceList.length;
    var list = '[';
    for(var i = 0;i<serviceNumber;i++){
        var item = '{';
        item = item + '"category": "' + serviceList[i][0] + '",';//服务分类
        item = item + '"name": "' + serviceList[i][1] + '",';//服务名称
        item = item + '"logo": "' + serviceList[i][2] + '"}';//服务logo
        if(i != serviceNumber-1)
            item += ',';
        list += item;
    }
    list += ']';
    return JSON.parse(list);
}

app.get('/home',function(req,res){    //服务首页
    var ans = serverHome(); 
    res.send(ans);
});
app.get('/bank',function(req,res){    //app首页显示银行的部分
    var ans = showBankInfo();
    res.send(ans);
});
app.get('/select',function(req,res){   //选择银行页面
    var ans = selectBank();
    res.send(ans);
});
app.get('/allService',function(req,res){ //全部服务页面
    var ans = allService();
    res.send(ans);
});
app.listen(3333,function(){
    console.log('listening 3333');
});