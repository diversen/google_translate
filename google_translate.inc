<?php

/**
 * function for translating system.inc and language.inc
 * to other languages with google translate api.
 *
 * @param array $options
 */
function google_translate($options){

    $g_options = google_translate_set_options($options);
    gtranslate::setOptions($g_options);

    if (isset($options['template'])) {
        $module_dir = _COS_HTDOCS . "/templates/$options[module]";
    } else {
        $module_dir = _COS_PATH . "/modules/$options[module]";
    }
    
    if (!file_exists($module_dir)) {
        cos_cli_print_status('Error', 'r', 'No such module dir: ' . $module_dir);
        return;
    }

    $source_dir = $module_dir . "/lang/$options[source]";
    $target_dir = $module_dir . "/lang/$options[target]";
    
    if (!file_exists($source_dir)){
        cos_cli_print_status('Error', 'r', "Source dir does not exists: $source_dir");
        return;
    }

    if (!file_exists($target_dir)){
        mkdir($target_dir);
    }

    cos_cli_print_status('OK', 'g', "Will translate $options[module]");
    $_COS_LANG_MODULE = array ();
    include "$source_dir/language.inc";
    
    $str = "<?php\n\n";
    cos_cli_print("Please wait. Translating.");
    foreach ($_COS_LANG_MODULE as $key => $val) {
        $trans = gtranslate::translateSingle($val);
        if (!$trans) {
            cos_cli_print_status('NOTICE', 'y', "Notice: Could not translate $val");
        } else {
            echo "$val\n$trans\n---\n";
        }
        $str.= "\$_COS_LANG_MODULE['$key'] = '$trans';" . "\n";
        sleep(1);
    }

    file_put_contents($target_dir . "/language.inc", $str);

    unset($_COS_LANG_MODULE);
    include "$source_dir/system.inc";
    
    if (!isset($_COS_LANG_MODULE)) {
        cos_cli_print_status('NOTICE', 'y', "Notice: No strings in system.inc");
        return;
    }

    $str = "<?php\n\n";
    foreach ($_COS_LANG_MODULE as $key => $val) {
        $trans = gtranslate::translateSingle($val);
        if (!$trans) {
            cos_cli_print_status('NOTICE', 'y', "Notice: Could not translate $val");
        }else {
            echo "$val\n$trans\n---\n";
        }
        $str.= "\$_COS_LANG_MODULE['$key'] = '$trans';" . "\n";
        sleep(1);
    }
    
    file_put_contents($target_dir . "/system.inc", $str);
    cos_cli_print_status('OK', 'y', "Translation complete of module $options[module]");
}

function google_translate_template($options) {
    $options['template'] = true;
    google_translate($options);
}


/**
 * function for translating system.inc and language.inc
 * to other languages with google translate api.
 *
 * @param array $options
 */
function google_translate_update($options){

    $g_options = google_translate_set_options($options);
    gtranslate::setOptions($g_options);

    if (isset($options['template'])) {
        $module_dir = _COS_HTDOCS . "/templates/$options[module]";
    } else {
        $module_dir = _COS_PATH . "/modules/$options[module]";
    }
    
    if (!file_exists($module_dir)) {
        cos_cli_print_status('Error', 'r', 'No such module dir: ' . $module_dir);
        return;
    }

    $source_dir = $module_dir . "/lang/$options[source]";
    $target_dir = $module_dir . "/lang/$options[target]";
    
    if (!file_exists($source_dir)){
        cos_cli_print_status('Error', 'r', "Source dir does not exists: $source_dir");
        return;
    }

    if (!file_exists($target_dir)){
        mkdir($target_dir);
    }
    
    $org_lang_file = $target_dir . "/language.inc";
    $_COS_ORG_LANG = array ();
    if (file_exists($org_lang_file)) {    
        include_once $org_lang_file;
        $_COS_ORG_LANG = $_COS_LANG_MODULE;
    }

    cos_cli_print_status('OK', 'g', "Will translate $options[module]");
    $_COS_LANG_MODULE = array ();
    include "$source_dir/language.inc";
    
    $str = "<?php\n\n";
    cos_cli_print("Please wait. Translating.");
    foreach ($_COS_LANG_MODULE as $key => $val) {
        
        // skip if already translated
        if (isset($_COS_ORG_LANG[$key]) ) {
            $org_val = $_COS_ORG_LANG[$key];
            $str.= "\$_COS_LANG_MODULE['$key'] = '$org_val';" . "\n";
            continue;
        }
        
        $trans = gtranslate::translateSingle($val);
        if (!$trans) {
            cos_cli_print_status('NOTICE', 'y', "Notice: Could not translate $val");
        } else {
            echo "$val\n$trans\n---\n";
        }
        $str.= "\$_COS_LANG_MODULE['$key'] = '$trans';" . "\n";
        sleep(1);
    }

    file_put_contents($target_dir . "/language.inc", $str);

    $org_system_file = $target_dir . "/system.inc";
    if (file_exists($org_system_file)) {
        $_COS_ORG_SYSTEM = array ();
        include_once $org_system_file;
        $_COS_ORG_SYSTEM = $_COS_LANG_MODULE;
    }
    
    
    unset($_COS_LANG_MODULE);
    include "$source_dir/system.inc";
    
    if (!isset($_COS_LANG_MODULE)) {
        cos_cli_print_status('NOTICE', 'y', "Notice: No strings in system.inc");
        return;
    }

    $str = "<?php\n\n";
    foreach ($_COS_LANG_MODULE as $key => $val) {
        
        // check if is translated
        if (isset($_COS_ORG_SYSTEM[$key])) {
            $str.= "\$_COS_LANG_MODULE['$key'] = '$val';" . "\n";
            continue;
        } 
        
        $trans = gtranslate::translateSingle($val);
        if (!$trans) {
            cos_cli_print_status('NOTICE', 'y', "Notice: Could not translate $val");
        }else {
            echo "$val\n$trans\n---\n";
        }
        $str.= "\$_COS_LANG_MODULE['$key'] = '$trans';" . "\n";
        sleep(1);
    }
    
    file_put_contents($target_dir . "/system.inc", $str);
    cos_cli_print_status('OK', 'y', "Translation complete of module $options[module]");
}

/**
 * update a template translation
 * @param array $options options given from shell
 */
function google_translate_template_update($options) {
    $options['template'] = true;
    google_translate_update($options);
}

/**
 * parsed options given from shell into google options
 * @param array $options
 * @return array ½options google options
 */
function google_translate_set_options (&$options) {
    $g_options = array();
    $g_options['key'] = config::getModuleIni('google_translate_key');

    if (empty($options['source'])){
        $options['source'] = 'en_GB';
        $g_options['source'] = 'en';
    } else {
        $g_options['source'] = $options['source'];
        $ary = explode('_', $options['source']);
        
        // da_DK e.g. we only need 'da' in google translate API
        if (count($ary) == 2) {
            $g_options['source'] = $ary[0];
        }
    }

    $g_options['target'] = $options['target'];
    return $g_options;
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
    $g_options['key'] = config::getModuleIni('google_translate_key');
    gtranslate::setOptions($g_options);
    $ary = gtranslate::getSupportLangs();
    return $ary;

}

function google_translate_all_mods ($options) {
    if (!isset($options['module']) && $options['module'] != 'all') {
        cos_cli_abort("specify 'all' as module when translating all modules");
    }
    $mods = moduleloader::getAllModules();
    foreach ($mods as $mod) {
        $options['module'] = $mod['module_name'];
        google_translate_update($options);
    }
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

mainCli::setOption('google_translate_update', array(
    'long_name'   => '--update',
    'short_name'   => '-u',
    'description' => 'Updates a translation. Use short code for target and source language (source must exist in module/lang folder',
    'action'      => 'StoreTrue'
));

mainCli::setOption('google_translate_template', array(
    'long_name'   => '--temp',
    'description' => 'Create a translation file from all strings that should be translated. Use short code for target and source language (source must exist in module/lang folder',
    'action'      => 'StoreTrue'
));

mainCli::setOption('google_translate_template_update', array(
    'long_name'   => '--temp-up',
    'description' => 'Updates a translation. Use short code for target and source language (source must exist in module/lang folder',
    'action'      => 'StoreTrue'
));

mainCli::setOption('google_translate_all_mods', array(
    'long_name'   => '--all-up',
    'description' => 'Translate all modules into a language',
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
