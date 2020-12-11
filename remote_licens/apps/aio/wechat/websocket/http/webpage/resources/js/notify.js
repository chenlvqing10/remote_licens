function notify(text, sticky) {
    $().toastmessage('showToast',{
        text: text,
        sticky   : sticky || false,
        position : 'top-right',
        closeText: ''
    });
}

function check_permission() {
    if (typeof ($.cookie("pwd")) == "undefined"){
        location.href = "index.html"
    }
}
