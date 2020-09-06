"use strict";
/*
*   ajax POST调用
* */
function AjaxPost(_url,_paramter,_success_func,_error_func,_dataType,_async) {
    $.ajax({
        type:'POST',
        url:_url,
        cache:false,
        async:_async,
        dataType:_dataType,
        data:_paramter,
        success:function(data) {
            _success_func(data);
        },
        error:function(XMLHttpRequest, textStatus, errorThrown) {
            _error_func(textStatus);
        }
    });
};
/*
*   数组转ztree List
* */
function ListToMap(data){
    var setting = {
        data: {
            simpleData: {
                enable: true
            }
        }
    };
    var t = $("#test");
    t = $.fn.zTree.init(t, setting, data);
    return t.getNodes();
}



/*
*   获取页面参数
* */
function GetUrlParam(){
    if(!window.location.search)return{};
    var str=window.location.search.substr(1),k,obj={};
    str='{"'+str.replace(/=/g, '":"').replace(/&/g, '","')+'"}';
    for (k in JSON.parse(str)){
        obj[k]=decodeURI(JSON.parse(str)[k]);
    }
    return obj;
}
/*
*   对象转url参数
* */
function JsonToUrlParams(_data){
    var k,str="";
    for(k in _data){str+=k+"="+_data[k]+"&";}
    return str.substr(0,str.length-1);
}
/*
*   lay弹出层
* */
function OpenLayer(_url,_title,_width,_height){
    var bWidth="80%";
    var bHeight="80%";
    if(_width)bWidth=_width;
    if(_height)bHeight=_height;
    layer.open({
        type: 2,
        title: _title,
        closeBtn: 1,
        shadeClose: false,
        skin: 'aaaa',                //自定义class
        maxmin: true,                //开启最大化最小化按钮
        area: [bWidth, bHeight],
        content: _url,
        cancel: function(){
            //右上角关闭回调
            //window.parent.location.reload();
        }
    });
}

/**
    日期格式化 _fmt : "yyyy年MM月dd日 hh时mm分ss秒SS q季度 周W"
    FORMAT(object,format) : 返回object的format格式。
    object 需要被格式化对象，可以是String，数字，Object(常用的有Date, Time)。
    format 格式化的样式。
    示例
    FORMAT(1234.5, "#,##0.00")               => 1234.50
    FORMAT(1234.5, "#,##0")                  => 1234
    FORMAT(1234.5, "￥#,##0.00")             => ￥1234.50
    FORMAT(1.5, "0%")                        => 150%
    FORMAT(1.5, "0.000%")                    => 150.000%
    FORMAT(date(2007,1,1), "EEEEE, MMMMM dd, yyyy") => 星期一，一月 01，2007
    FORMAT(date(2007,1,13), "MM/dd/yyyy") => 01/13/2007
    FORMAT(date(2007,1,13), "M-d-yy") => 1-13-07
    FORMAT(time(16,23,56), "h:mm:ss a") => 4:23:56 下午
    console.log(Format(ToDate(),"yyyy年MM月dd日 hh时mm分ss秒SS q季度 周W a"));
    console.log(Format(ToDate("2018-02-03 12:59:59"),"yyyy年MM月dd日 hh时mm分ss秒SS q季度 周W a"));
    console.log(Format(12345678956.23,"总计:#,###0.000 ￥"));
    console.log(Format("123456.789","合计:#,###0.00万元"));
    console.log(Format(1.456534,"上升0%"));
    console.log(Format("0.51","下降0.00%"));
 */
function ToDate (_data){
    if(!_data){return new Date()}else{return new Date(_data)}
};
function Format(_data,_fmt) {
    //日期  0转日
    function numTohz(_val){
        var arr={0:"日",1:"一",2:"二",3:"三",4:"四",5:"五",6:"六"};
        return arr[_val]
    }
    //判断上下午
    function isAmPm(_val){
        return _val>=12?"下午":"上午"
    }
    //时间格式化
    if(typeof(_data)==="object"){
        var o = {
            "M+": _data.getMonth() + 1,       //月份
            "d+": _data.getDate(),            //日
            "h+": _data.getHours(),           //小时
            "m+": _data.getMinutes(),         //分
            "s+": _data.getSeconds(),         //秒
            "S+": _data.getMilliseconds(),    //毫秒
            "a+": isAmPm(_data.getHours()),   //上下午
            "W+": numTohz(_data.getDay()),    //周几
            "q+": numTohz(Math.floor((_data.getMonth() + 3) / 3)) //季度
        };
        if (/(y+)/.test(_fmt)) _fmt = _fmt.replace(RegExp.$1, (_data.getFullYear() + "").substr(4 - RegExp.$1.length));
        for (var k in o)
            if (new RegExp("(" + k + ")").test(_fmt)) _fmt = _fmt.replace(RegExp.$1, (RegExp.$1.length == 1) ? (o[k]) : (("00" + o[k]).substr(("" + o[k]).length)));
    }
    //金额格式化
    else{
        var a=0;  //金额间隔
        var b=0;  //保留几位
        var c=""; //fmt前
        var d=""; //fmt后
        _fmt.indexOf(",")>=0?a=_fmt.lastIndexOf("#")-_fmt.indexOf(","):a=0;
        _fmt.indexOf(".")>=0?b=_fmt.lastIndexOf("0")-_fmt.indexOf("."):b=0;
        if(_fmt.indexOf("%")>0){
            c = _fmt.substr(0, _fmt.indexOf("0"));
            d = _fmt.substr(_fmt.lastIndexOf("%") + 1, _fmt.length);
            _data=parseFloat(_data*100).toFixed(b+1);
            _fmt=c+parseFloat(_data).toFixed(0)+"%"+d;
        }else{
            c = _fmt.substr(0, _fmt.indexOf("#"));
            _fmt.lastIndexOf("0")>=0?
            d = _fmt.substr(_fmt.lastIndexOf("0") + 1, _fmt.length):
            d = _fmt.substr(_fmt.lastIndexOf("#") + 1, _fmt.length);
            _data=parseFloat(_data).toFixed(b+1);
            var str = _data.toString().split('');
            str.splice(str.indexOf(".") + b + 1);
            str = str.join('');
            var numInt = str.match(new RegExp(/\d+/g))[0];
            var numFot = str.match(new RegExp(/\d+/g))[1] ? str.match(new RegExp(/\d+/g))[1] : '';
            numInt = numInt.split('').reverse().join('').match(new RegExp("\\d{" + 1 + "," + a + "}", "g")).join(",").split('').reverse().join('');
            b > 0 ? _fmt = c + numInt + "." + numFot + d : _fmt = c + numInt + numFot + d;
        }
    }
    return _fmt;
};
/*
*   首页设置标签位置
* */
function SetTabsPosition(el){
    var gap=56;  //最后一个余量
    var tabBxWidth=$(".ns-tab-box").width();
    var tabUlWidth=$(".ns-tab-box ul").width();
    var tabUlLeft=parseInt($(".ns-tab-box ul").css("left"));
    var tabLiLeft=0;
    var tabLiwidth=$(".ns-tab-box li.after").width();
    for(var i=0;i<$(".ns-tab-box li.after").index();i++){
        tabLiLeft+=$($(".ns-tab-box li")[i]).width()+1;
    }
    if(tabUlWidth>tabBxWidth){
        if(el==="next"){
            var newnextLeft=0;
            var liIndex=0;
            for(var j=0;j<$(".ns-tab-box li").length;j++){
                if(newnextLeft<-tabUlLeft+tabBxWidth){
                    newnextLeft+=$($(".ns-tab-box li")[j]).width()+1;
                    liIndex=j;
                }
            }
            newnextLeft=newnextLeft-$($(".ns-tab-box li")[liIndex]).width()-1;
            if(newnextLeft+tabBxWidth>tabUlWidth){newnextLeft=tabUlWidth-tabBxWidth+gap}
            $(".ns-tab-box ul").css("left",-newnextLeft);
        }else if(el==="prev"){
            var newPrevLeft=0;
            for(var k=0;k<$(".ns-tab-box li").length;k++){
                if(newPrevLeft<-tabUlLeft){newPrevLeft+=$($(".ns-tab-box li")[k]).width()+1;}
            }
            newPrevLeft=newPrevLeft-tabBxWidth;
            if(newPrevLeft<=0){newPrevLeft=0}
            $(".ns-tab-box ul").css("left",-newPrevLeft);
        }else if(el==="close"){
            if(tabUlWidth-(-tabUlLeft)+gap<tabBxWidth){
                $(".ns-tab-box ul").css("left",-tabUlWidth+tabBxWidth-gap);
            }
        }else{
            //新增||点击||切换
            if(tabLiLeft<-tabUlLeft){
                $(".ns-tab-box ul").css("left",-tabLiLeft);
            }else if(tabLiLeft+tabLiwidth>(-tabUlLeft)+tabBxWidth){
                $(".ns-tab-box ul").css("left",-(tabLiLeft-tabBxWidth+tabLiwidth+gap));
            }
        }
    }else{
        $(".ns-tab-box ul").css("left",0)
    }
}
/*
*   首页点击侧边栏添加标签
* */
function SetTabsAndIframe(_this){
    $(".ns-btn-home").removeClass("after");
    $(".ns-tab-box li").removeClass("after");
    $(".ns-body .ns-body-item").removeClass("show");
    var pageTitle=$(_this).children("cite").html();
    var pageUrl=$(_this).attr("ns-href");
    var hasTabs=false;
    var tabsHtml="<li class='after'><a ns-href='"+pageUrl+"'>"+pageTitle
        +"</a><span class='layui-icon layui-icon-close'></span></li>";
    var ifrmHtml=" <div class='ns-body-item show'><iframe src='"+pageUrl+"'></iframe></div>";
    if($(".ns-tab-box a[ns-href='"+pageUrl+"']").length===1)hasTabs=true;
    //主页
    if($(_this).parents("li").index()===0){
        $(".ns-btn-home").addClass("after");
        $(".ns-body .ns-body-item:first-child").addClass("show");
        //新加
    }else if(!hasTabs){
        $(".ns-tab-box ul").append(tabsHtml);
        $(".ns-body").append(ifrmHtml);
        SetTabsPosition();
        //重复
    }else {
        for(var j=0;j<$(".ns-tab-box li").length;j++){
            if($($(".ns-tab-box a")[j]).attr("ns-href")===pageUrl){
                $($(".ns-tab-box li")[j]).addClass("after");
                $($(".ns-body .ns-body-item")[j+1]).addClass("show");
            }
        }
        SetTabsPosition();
    }
}
/*
*   首页点击标签切换
* */
function SetSideAndIframe(_this){
    var pageUrl=$(_this).attr("ns-href");
    $(".ns-nav a").removeClass("on");
    $(".ns-btn-home").removeClass("after");
    $(".ns-tab-box li").removeClass("after");
    $(".ns-body .ns-body-item").removeClass("show");
    $(_this).parent().addClass("after");
    $(".ns-nav a[ns-href='"+pageUrl+"']").addClass("on");
    $("iframe[src^='"+pageUrl+"']").parent().addClass("show");
    SetTabsPosition();
}
/*
*   首页关闭标签
* */
function ColseTab(el){
    function CloseAll(){
        $(".ns-btn-home").addClass("after");
        $(".ns-body-item:first-child").addClass("show");
        $(".ns-nav a").removeClass("on");
        $(".ns-nav li:first-child a").addClass("on");
        $(".ns-tab-box li").remove();
        $(".ns-body-item:not(:first-child)").remove();
    }
    if(el==="all"){
        CloseAll();
    }else if(el==="other"){
        $(".ns-tab-box li:not(.after)").remove();
        $(".ns-body-item:not(:first-child,.show)").remove();
    }else{
        if($(".ns-btn-home").hasClass("after"))return;
        if($(el).parent().hasClass("after")||el==="this"){
            var liNextLength=$(".ns-tab-box li.after").next().length;
            var liPrevLength=$(".ns-tab-box li.after").prev().length;
            var oldLiIndex=$(".ns-tab-box li.after").index();
            var newLiIndex=oldLiIndex;
            if(liNextLength===0&&liPrevLength===0){
                CloseAll();
            }else if(liNextLength===1){
                newLiIndex++;
            }else{
                newLiIndex--
            }
            $($(".ns-tab-box li")[newLiIndex]).addClass("after");
            $($(".ns-tab-box li")[oldLiIndex]).remove();
            $($(".ns-body-item")[newLiIndex+1]).addClass("show");
            $($(".ns-body-item")[oldLiIndex+1]).remove();
            if($(".ns-tab-box li.after a").attr("ns-href")){
                $(".ns-nav a").removeClass("on");
                $(".ns-nav a[ns-href='"+$(".ns-tab-box li.after a").attr("ns-href")+"']").addClass("on");
            }
        }else{
            var ifIndex=$(el).parent().index();
            $($(".ns-tab-box li")[ifIndex]).remove();
            $($(".ns-body-item")[ifIndex+1]).remove();
        }
    }
    SetTabsPosition("close");
}
/*
*   首页不同尺寸侧边样式
* */
function SetSideStyle(){
    $("#ns-layout").toggleClass("ns-side-fold");
    $(".ns-nav li>dl").removeClass("show");
    if($(window).width()>1200){
        if($(".ns-side-fold").length===0){
            $(".ns-nav li>dl[isshow=true]").addClass("show");
        }
    }else if($(window).width()>768){
        if($(".ns-side-fold").length===1){
            $(".ns-nav li>dl[isshow=true]").addClass("show");
        }
    }else{
        if($(".ns-side-fold").length===1){
            $(".ns-nav li>dl[isshow=true]").addClass("show");
        }
    }
}
/*
*   iframe页面内跳转 _title=目录标题,必须一致;
*   Jump('页面1.html?id=1','页面1')
* */
function Jump(_url,_title){
    var newUrl="";
    _url.indexOf("?")>0?newUrl=_url.substr(0,_url.lastIndexOf("?")):newUrl=_url;
    var $tablis=$(".ns-tab-box li,.ns-btn-home",parent.document);
    var $sideAs=$(".ns-nav a",parent.document);
    var $iframs=$(".ns-body-item",parent.document);
    var $tablisHtml="<li class='after'><a ns-href='"+newUrl+"'>"+_title+"</a>" +
                    "<span class='layui-icon layui-icon-close'></span></li>";
    var $iframsHtml="<div class='ns-body-item show'><iframe src='"+_url+"'></iframe></div>";
    $tablis.removeClass("after");
    $sideAs.removeClass("on");
    $iframs.removeClass("show");
    if($(".ns-tab-box a[ns-href='"+newUrl+"']",parent.document).length===1){
        $(".ns-tab-box a[ns-href='"+newUrl+"']",parent.document).parent().addClass("after");
        $(".ns-body-item iframe[src^='"+newUrl+"']",parent.document).parent().addClass("show");
        $(".ns-body-item iframe[src^='"+newUrl+"']",parent.document).attr('src',_url);
    }else{
        $(".ns-tab-box ul",parent.document).append($tablisHtml);
        $(".ns-body",parent.document).append($iframsHtml);
    }
    $(".ns-nav a[ns-href='"+newUrl+"']",parent.document).addClass("on");
    window.parent.SetTabsPosition();
}









