//页面初始化
$(function(){
    $("input[name=username]").val("");
    $("input[name=password]").val("");
    $("input[name=vercode]").val("");
});
layui.use('form', function(){
    var timer1=null;
    clearTimeout(timer1);
    var show_num = [];
    draw(show_num);
    $("#vercode").on('click',function(){
        draw(show_num);
    });
    function longin(){
        clearTimeout(timer1);
        timer1=null;
        $("#login-submit").attr("disabled",true);
        var username=$("input[name=username]").val();
        var password=$("input[name=password]").val();
        var vercode=$("input[name=vercode]").val().toLowerCase();
        var num = show_num.join("");

        var remember=$("input[name=remember]").next().hasClass("layui-form-checked");
        //添加 layui-form-danger
        function clearMsgStyle(){
            $(".msg").addClass("shake").show();
            timer1=setTimeout(function(){
                $(".msg").hide();
                $("#login-submit").removeAttr("disabled");
            },1700);
        }
        $("input").blur(function(){
            $("input").removeClass("layui-form-danger")
        });
        if(!username){
            $("input[name=username]").addClass("layui-form-danger").focus();
            $(".msg p").html("用户名不能为空");
            clearMsgStyle()
        }else if(!password){
            $("input[name=password]").addClass("layui-form-danger").focus();
            $(".msg p").html("密码不能为空");
            clearMsgStyle()
        }else if(!vercode){
            $("input[name=vercode]").addClass("layui-form-danger").focus();
            $(".msg p").html("验证码不能为空");
            clearMsgStyle()
        }else if(vercode!==num) {
            $("input[name=vercode]").addClass("layui-form-danger").focus().val("");
            $(".msg p").html("验证码错误");
            draw(show_num);
            clearMsgStyle()
        }else{
            /* $("input[name=username]").val("");
             $("input[name=password]").val("");
             $("input[name=vercode]").val("");*/
            /* $(".msg").show();
             $(".msg p").html("登录成功");*/
            window.location.href=window.location.href.substr(0,window.location.href.lastIndexOf("/")+1)+"index.html";
        }
    }
    $(window).keydown(function(e){
        if(e.keyCode===13){
            longin()
        }
    });
    $("#login-submit").click(function(){
        longin();
    })
});