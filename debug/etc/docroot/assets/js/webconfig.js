var webInfo={
    "webname":"机动车监管平台",
    "weblogo":"asd",
    "copyright":"© 2018 XXXXXXXXXXXX<br>new sun",
    "setIndex":function(){
        $(".ns-logo-text").html(this.webname);
        $("title").html(this.webname);
    },
    "setLogin":function(){
        $("#copyright").html(this.copyright);
        $("title").html(this.webname);
    },
    "start":function(){
        this.setIndex();
        this.setLogin();
    }
};
webInfo.start();