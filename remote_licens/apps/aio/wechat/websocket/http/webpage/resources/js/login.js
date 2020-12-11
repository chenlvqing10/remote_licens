$(function() {
	// Waves初始化
	Waves.displayEffect();
	// 输入框获取焦点后出现下划线
	$('.form-control').focus(function() {
		$(this).parent().addClass('fg-toggled');
	}).blur(function() {
		$(this).parent().removeClass('fg-toggled');
	});
});
Checkbix.init();
$(function() {
	// 点击登录按钮
	$('#login-bt').click(function() {
		login();
	});
	// 回车事件
	$('#username, #password').keypress(function (event) {
		if (13 == event.keyCode) {
			login();
		}
	});
});
// 登录
function login() {
	$.ajax({
		url:  '/deviceLogin',
		type: 'POST',
		contentType:"application/json",
		dataType: "json",
		data: JSON.stringify({
			username: $('#username').val(),
			password: $('#password').val(),
			rememberMe: $('#rememberMe').is(':checked'),
			// backurl: BACK_URL
		}),
		beforeSend: function() {

		},
		success: function(json){
			console.log("login ok",json)
			if (json.result == 0) {
				console.log("login ok")
				location.href = "home.html";
				$.cookie("pwd",$('#password').val())
			} else {
				alert(json.data);
				console.log(json, json.data)
				if (10101 == json.code) {
					$('#username').focus();
				}
				if (10102 == json.code) {
					$('#password').focus();
				}
			}
		},
		error: function(error){
			console.log("error::::",error);
			let resp = JSON.parse(error.responseText)

			if (resp.result == -5){
				//notify("login user name error")
				notify(window.lang.username_error)
			}else if (resp.result == -1){
				//notify("login password error")
				notify(window.lang.password_error)
			}else{
				//notify("other login error")
				notify(window.lang.unknown_error)
			}
		}
	});
}