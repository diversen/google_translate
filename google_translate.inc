<?php


/**
 * function for translating system.inc and language.inc
 * to other languages with google translate api.
 *
 * @param array $options
 */
function google_translate($options){
    //include_once "GTranslate.php";

    $g_options = array();
    $g_options['key'] = mainCli::$ini['google_translate']['key'];

    if (empty($options['source'])){
        $options['source'] = 'en_GB';
        $g_options['source'] = 'en';
    } else {
        $g_options['source'] = $options['source'];
    }

    $g_options['target'] = $options['target'];
    gtranslate::setOptions($g_options);

    $module_dir = _COS_PATH . "/modules/$options[module]";
    if (!file_exists($module_dir)){
        cos_cli_abort('No such module dir: ' . $module_dir);
    }

    $source_dir = $module_dir . "/lang/$options[source]";
    if (!file_exists($source_dir)){
        cos_cli_abort("Error: Source dir does not exists: $source_dir");
    }

    $target_dir = $module_dir . "/lang/$options[target]";
    if (!file_exists($target_dir)){
        mkdir($target_dir);
    }

    include "$source_dir/language.inc";
    
    $str = "<?php\n\n";
    cos_cli_print("Please wait. Translating.");
    foreach ($_COS_LANG_MODULE as $key => $val) {
        $trans = gtranslate::translateSingle($val);
        if (!$trans) {
            cos_cli_print ("Notice: Could not translate $val");
        }
        $str.= "\$_COS_LANG_MODULE['$key'] = '$trans';" . "\n";
        echo ".";
    }

    file_put_contents($target_dir . "/language.inc", $str);

    unset($_COS_LANG_MODULE);
    include "$source_dir/system.inc";

    $str = "<?php\n\n";
    foreach ($_COS_LANG_MODULE as $key => $val) {
        $trans = gtranslate::translateSingle($val);
        if (!$trans) {
            cos_cli_print ("Notice: Could not translate $val");
        }
        $str.= "\$_COS_LANG_MODULE['$key'] = '$trans';" . "\n";
        echo ".";
    }
    file_put_contents($target_dir . "/system.inc", $str);
    cos_cli_print('Translation complete!');
}

function google_translate_all ($options){
    $ary = google_translate_get_langs($options);
    if (empty($options['module'])){
        cos_cli_abort("Specify a module");
    }

    foreach ($ary['data']['languages'] as $key => $val){
        cos_cli_print("Translating into $val[name]");
        $options['target'] = $val['language'];
        google_translate($options);
    }
}


/**
 * function for getting an assoc array with all supported langs
 * where target is en.
 * @param array $options
 */
function google_translate_langs($options){
    $ary = google_translate_get_langs($options);
    print_r($ary);
}

function google_translate_get_langs($options){
    //include_once "GTranslate.php";

    $g_options = array();
    $g_options['target'] = "en";
    $g_options['key'] = mainCli::$ini['google_translate']['key'];
    gtranslate::setOptions($g_options);
    $ary = gtranslate::getSupportLangs();
    return $ary;

}

mainCli::setCommand('google_translate', array(
    'description' => 'Will create a translation file for specified language for a module using googles translate api',
));

mainCli::setOption('google_translate', array(
    'long_name'   => '--translate',
    'short_name'   => '-t',
    'description' => 'Create a translation file from all strings that should be translated. Use short code for target and source language (source must exist in module/lang folder',
    'action'      => 'StoreTrue'
));

mainCli::setOption('google_translate_all', array(
    'long_name'   => '--all-translate',
    'short_name'   => '-a',
    'description' => 'Translate a module to all known google languages with english (en_GB) as source',
    'action'      => 'StoreTrue'
));

mainCli::setOption('google_translate_langs', array(
    'long_name'   => '--langs',
    'short_name'  => '-l',
    'description' => 'Show all available google languages. The short code should be used as target',
    'action'      => 'StoreTrue'
));

mainCli::setArgument('module',
    array('description'=> 'Specicify the module for which you will make a translation',
          'optional' => true));

mainCli::setArgument('target',
    array('description'=> 'Specicify the target language which we will translate into',
          'optional' => true));

mainCli::setArgument('source',
    array('description'=> 'Specicify the source language which we will translate from. If not specified en_GB will be used',
          'optional' => true));
