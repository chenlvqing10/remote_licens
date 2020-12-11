let langList = ["cn", "en", "it", "sn", "tr", "ru", "ab", "pl", "jp", "fh"];
let current_lang = 0

// $(document).ready(function() {
//
//     let langJs = langList[lang_idx()];
//
//     $.cookie('lang', langJs, {
//         'hoursToLive': 24 * 365
//     });
//     lang_load()
// });

function lang_idx() {
    let lang_index = 0
    let lang = $.cookie('lang')
    if(typeof (lang) == "undefined") {
        console.log("no lang in cookie " )
    }else{
        langList.map(function (value, idx, langList) {
            if (value == lang){
                lang_index = idx
            }
        })
    }
    return lang_index
}

function lang_load_if_change() {
    if (current_lang != lang_idx()){
        current_lang = lang_idx()
        lang_change(current_lang)
    }
}
function lang_load() {
    lang_change(lang_idx())
}

function lang_change(lang_idx) {
    $.cookie('lang', langList[lang_idx], {
        'hoursToLive': 24 * 365
    });
    let langJs = langList[lang_idx];
    current_lang = lang_idx
    console.log("language:", langJs)
    $.ajax({
        url:"resources/lang/" + langJs + ".js",
        dataType:"script",
        async: false,
        success:function () {
            $('.lang').each(function (idx, ele) {
                // console.log(idx, ele)
                let key = ele.getAttribute("key")
                let value = lang[key]
                window.lang = lang
                //console.log(key, ele.innerText, value)
                if (typeof (value) != "undefined"){
                    $(this).text(value)
                }
            })
            // console.log($('.content_main'))
            // $('.content_main').contents().find(".iframe").each(function (idx ,ele) {
            //         console.log("content_main", idx, ele)
            //     }
            // )
            console.log("reload to refresh lang")
            // reload to refresh lang
            //window.location.href = window.location.href;
        },
        always:function () {
            console.log("always lang")
        },
        error: function (error) {
                console.log("error lang", error)
            },
    })

};