function createInfo(file, newat, altnames = '', removedat = '') {
    this.file = file;
    this.newat = newat;
    this.altnames = altnames;
    this.removedat = removedat;
}

const versionsArr = ['0.0'];
const unsupportedArr = [ ];
var titles = [
    {k: 'en', v: ['Supported versions', 'Unsupported versions']},
];


var newpages = [
    {v: '0.1', pages: [
        'concepts', 'functions', 'index', 'installation', 'introduction',
        'release_notes', 'sampledata', 'support',
        'vrp_full_version', 'vrp_version',
        'pgr-category.rst', 'vrp_oneDepot.rst',
        'vrp_pgr_pickDeliver.rst', 'vrp_pgr_pickDeliverEuclidean.rst'
    ] },
];

var filesArr = [ ];

function get_titles(data, k) {
    for (var i = 0; i < data.length; i++) {
        if (data[i].k == k) { return data[i]; }
    }
    return '';
}

function get_vdata(data, v, file, lang) {
    let info = '';
    let link = '';
    for (var i = 0; i < data.length; i++) {
        if (v <= data[i].v) { info = data[i]; break; }
    }
    if (info === '') {
        link = `${lang}/${file}.html">${v}</a>`;
    } else {
        link = `${lang}/${info.n}.html">${v}</a>`;
    }
    return link;
}

function get(name) {
    for (var i = 0; i < filesArr.length; i++) {
        if (filesArr[i].file === name) { return filesArr[i]; }
    }
}

function prepare() {
    for (var i = 0; i < newpages.length; i++) {
        for (var j = 0; j < newpages[i].pages.length; j++) {
            filesArr.push(new createInfo(newpages[i].pages[j], newpages[i].v));
        }
    }
}

function get_history(name, lang) {
    prepare();
    var data = get(name);
    var title = get_titles(titles, lang);
    var home = `<a class="reference external" href="https://pgvroom.pgrouting.org/`;
    var validlang = lang;

    var result;
    result = '<strong>' + title.v[0] + ':</strong>';
    result += home + 'latest/' + validlang + '/' + data.file + '.html"> latest </a> ';

    latest = versionsArr[0];
    for (var i = 0; i < versionsArr.length; i++) {
        validlang = lang;

        if (versionsArr[i] < data.newat) break;
        if (data.removedat != '' && versionsArr[i] > data.newat) break;

        /* for zh-Hans non translated versions use english */
        validlang = (lang == 'zh-Hans' && versionsArr[i] >= '3.7')? 'zh_Hans' : 'en';

        let link = `${home}${versionsArr[i]}/`
        link += get_vdata(data.altnames, versionsArr[i], data.file, validlang);
        if (versionsArr[i] == latest) result += '(' + link + ') ';
        else result += link + ' ';
    }
    result += home + 'main/' + lang + '/' + data.file + '.html"> main </a> ';
    result += home + 'dev/' + lang + '/' + data.file + '.html"> dev </a> ';

    if (data.newat <= unsupportedArr[0]) {
        result += '<br/><strong>' + title.v[1] + ':</strong>';
        for (var i = 0; i < unsupportedArr.length; i++) {
            /* basically 2.x has only english */
            validlang = 'en';

            /* for spanish non translated versions use english */
            validlang = (lang == 'es' && unsupportedArr[i] == '2.0')? 'es' : validlang;

            if (data.newat > unsupportedArr[i]) break;
            var link = home + unsupportedArr[i] + '/';
            link += get_vdata(data.altnames, unsupportedArr[i], data.file, validlang);
            result += link + ' ';
        }
    }
    return result;
}
