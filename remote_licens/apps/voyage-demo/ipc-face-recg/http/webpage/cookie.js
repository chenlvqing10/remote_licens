

function setCookie(name, value, sec){
	var exp = new Date();
	exp.setTime(exp.getTime()+sec*1000);
	document.cookie = name + "=" + escape(value) + ";expires=" + exp.toGMTString();
}

function getCookie(name){
	var arr,reg=new RegExp("(^|)"+name+"([^;]*)(;|$)");
	if(arr=document.cookie.match(reg)){
		return unescape(arr[2]);
	}else {
		return null;
	}
}

function delCookie(name){
	var exp = new Date();
	exp.setTime(exp.getTime() - 1);
	var val = getCookie(name);
	if(val != null){
		document.cookie = name + "=" + val + ";expires=" + exp.toGMTString();
	}
}
		
		