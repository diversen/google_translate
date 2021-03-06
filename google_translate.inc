<?php

use diversen\conf;
use diversen\gtranslate;
use diversen\layout;
use diversen\moduleloader;
use diversen\translate\extract;
/**
 * function for translating system.inc and language.inc
 * to other languages with google translate api.
 *
 * @param array $options
 */
function google_translate($options){

    $g_options = google_translate_set_options($options);
    gtranslate::setOptions($g_options);

    if (isset($options['vendor'])) {
        $module_dir = conf::pathBase() . "/vendor/diversen/simple-php-classes/src";
        $lang_dir = conf::pathBase() . "/vendor/diversen/simple-php-classes/src/lang/$options[language]";
    }
    elseif (isset($options['template'])) {
        $module_dir = conf::pathHtdocs() . '/templates/' . $options['module'];
        $lang_dir = conf::pathHtdocs() . "/templates/$options[module]/lang/$options[language]";
    } else {
        $module_dir = conf::pathModules() . "/$options[module]";
        $lang_dir = conf::pathModules() . "/$options[module]/lang/$options[language]";
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

    cos_cli_print_status('Notice', 'y', "Will translate $options[module]");
    $LANG = array ();
    include "$source_dir/language.inc";
    
    $str = "<?php\n\n";
    cos_cli_print("Please wait. Translating.");
    foreach ($LANG as $key => $val) {
        $trans = gtranslate::translateSingle($val);
        if (!$trans) {
            cos_cli_print_status('NOTICE', 'y', "Notice: Could not translate $val");
        } else {
            echo "$val\n$trans\n---\n";
        }
        $str.= extract::setCorrectQuotes($key, $trans);
        sleep(1);
    }

    file_put_contents($target_dir . "/language.inc", $str);

    unset($LANG);
    include "$source_dir/system.inc";
    
    if (!isset($LANG)) {
        cos_cli_print_status('NOTICE', 'y', "Notice: No strings in system.inc");
        return;
    }

    $str = "<?php\n\n";
    foreach ($LANG as $key => $val) {
        $trans = gtranslate::translateSingle($val);
        if (!$trans) {
            cos_cli_print_status('NOTICE', 'y', "Notice: Could not translate $val");
        }else {
            echo "$val\n$trans\n---\n";
        }
        $str.= extract::setCorrectQuotes($key, $trans);
        //$str.= "\$LANG['$key'] = '$trans';" . "\n";
        sleep(1);
    }
    
    file_put_contents($target_dir . "/system.inc", $str);
    cos_cli_print_status('OK', 'g', "Translation complete of module $options[module]");
}

function google_translate_template($options) {
    $options['template'] = true;
    google_translate($options);
}

function google_translate_vendor($options) {

    $options['vendor'] = true;
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

    if (isset($options['vendor'])) {
        $module_dir = conf::pathBase() . "/vendor/diversen/simple-php-classes/src";
    } elseif (isset($options['template'])) {
        $module_dir = conf::pathHtdocs() . '/templates/' . $options['module'];
    } else {
        $module_dir = conf::pathModules() . "/$options[module]";
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

        include $org_lang_file;
        if (isset($LANG)) {
            $_COS_ORG_LANG = $LANG;
        } else {
            $_COS_ORG_LANG = array ();
        }

    }

    cos_cli_print_status('OK', 'g', "Will translate $options[module]");
    $LANG = array ();
    
    if (file_exists("$source_dir/language.inc")) {
        include "$source_dir/language.inc";
    } else {
        cos_cli_print_status("NOTICE", 'y', "Source file: $source_dir/language.inc does not exists");
        return;
    }
    
    $str = "<?php\n\n";
    cos_cli_print("Please wait. Translating.");
    foreach ($LANG as $key => $val) {
        
        // skip if already translated
        if (isset($_COS_ORG_LANG[$key]) ) {
            $org_val = $_COS_ORG_LANG[$key];
            $str.= extract::setCorrectQuotes($key, $org_val);
            //$str.= "\$LANG['$key'] = '$org_val';" . "\n";
            continue;
        }
        
        $trans = gtranslate::translateSingle($val);
        if (!$trans) {
            cos_cli_print_status('NOTICE', 'y', "Notice: Could not translate $val");
        } else {
            echo "$val\n$trans\n---\n";
        }
        
        $str.= extract::setCorrectQuotes($key, $trans);
        //$str.= "\$LANG['$key'] = '$trans';" . "\n";
        sleep(1);
    }

    file_put_contents($target_dir . "/language.inc", $str);

    $org_system_file = $target_dir . "/system.inc";
    if (file_exists($org_system_file)) {
        $_COS_ORG_SYSTEM = array ();
        include $org_system_file;
        $_COS_ORG_SYSTEM = $LANG;
    }
    
    
    unset($LANG);
    if (file_exists("$source_dir/system.inc")) {
        include "$source_dir/system.inc";
    } else {
        cos_cli_print_status("NOTICE", 'y', "Source file: $source_dir/system.inc does not exists");
        return;
    }
    include "$source_dir/system.inc";
    
    if (!isset($LANG)) {
        cos_cli_print_status('NOTICE', 'y', "Notice: No strings in system.inc");
        return;
    }

    $str = "<?php\n\n";
    foreach ($LANG as $key => $val) {
        
        if (isset($_COS_ORG_SYSTEM[$key]) ) {
            $org_val = $_COS_ORG_SYSTEM[$key];
            // 
            $str.= extract::setCorrectQuotes($key, $org_val);
            continue;
        }
        
        $trans = gtranslate::translateSingle($val);
        if (!$trans) {
            cos_cli_print_status('NOTICE', 'y', "Notice: Could not translate $val");
        }else {
            echo "$val\n$trans\n---\n";
        }
        $str.= extract::setCorrectQuotes($key, html_entity_decode($trans, ENT_QUOTES));
        sleep(1);
    }
    
    file_put_contents($target_dir . "/system.inc", $str);
    cos_cli_print_status('OK', 'g', "Translation complete of module $options[module]");
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
 * update a template translation
 * @param array $options options given from shell
 */
function google_translate_vendor_update($options) {
    $options['vendor'] = true;
    google_translate_update($options);
}

/**
 * parsed options given from shell into google options
 * @param array $options
 * @return array ½options google options
 */
function google_translate_set_options (&$options) {
    $g_options = array();
    $g_options['key'] = conf::getModuleIni('google_translate_key');

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
    
    // set target 
    $ary = explode('_', $options['target']);
        
    // da_DK e.g. we only need 'da' in google translate API
    if (count($ary) == 2) {
        $g_options['target'] = $ary[0];
    } else {
        $g_options['target'] = $options['target'];
    }

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

    $g_options = array();
    $g_options['target'] = "en";
    $g_options['key'] = conf::getModuleIni('google_translate_key');
    gtranslate::setOptions($g_options);
    $ary = gtranslate::getSupportLangs();
    return $ary;

}

/**
 * updates all modules and templates for one language
 * @param array $options
 */
function google_translate_all_up ($options) {

    if (!isset($options['module']) && $options['module'] != 'all') {
        cos_cli_abort("specify 'all' as module when translating all modules");
    }
    
    $mods = moduleloader::getAllModules();
    foreach ($mods as $mod) {
        $options['module'] = $mod['module_name'];
        google_translate_update($options);
    }
    
    $mods = layout::getAllTemplates();
    foreach($mods as $mod) {
        $options['module'] = $mod;
        google_translate_template_update($options);
    }
}

function google_translate_all_from_ini ($options) {
    
    if (!isset($options['module']) && $options['module'] != 'all') {
        cos_cli_abort("specify 'all' as module when translating all modules");
    }
    
    $langs = conf::getModuleIni('google_translate_lang');
    foreach ($langs as $lang) {
        cos_cli_print_status('Notice',  'g',"Translating all from 'en_GB' to '$lang'" );
        $options['module'] = 'all';
        $options['target'] = $lang;
        $options['source'] = 'en_GB';
        google_translate_all_up($options);
    }
    return;
}

function google_translate_vendor_from_ini ($options) {
    
    $langs = conf::getModuleIni('google_translate_lang');
    foreach ($langs as $lang) {
        cos_cli_print_status('Notice',  'g',"Translating vendor from 'en_GB' to '$lang'" );
        $options['module'] = 'all';
        $options['target'] = $lang;
        $options['source'] = 'en_GB';
        google_translate_vendor_update($options);
    }
    return;
}

function google_translate_collect_all ($options) {
    
    $templates = layout::getAllTemplates();
    if (!in_array($options['module'], $templates)) {
        cos_cli_abort("No such template: $options[module]");
    }
    
    // extract translation from source language
    $options['language'] = $options['source'];
    translate_all_update($options);
    
    // translate all modules and templates
    $all_options = $options;
    $all_options['module'] = 'all';
    google_translate_all_mods($all_options);
    
    // collect all into one file for both source and target
    $collect_options = $options;
    $collect_options['language'] = $options['source'];
    translate_collect($collect_options);
    
    $collect_options['language'] = $options['target'];
    translate_collect($collect_options);
    
}

self::setCommand('google-translate', array(
    'description' => 'Translate using Googles Translate API',
));

self::setOption('google_translate', array(
    'long_name'   => '--translate',
    'short_name'   => '-t',
    'description' => 'Create a translation file from all strings that should be translated. Use short code for target and source language (source must exist in module/lang folder',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_update', array(
    'long_name'   => '--update',
    'short_name'   => '-u',
    'description' => 'Updates a translation. Use short code for target and source language (source must exist in module/lang folder',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_template', array(
    'long_name'   => '--temp',
    'description' => 'Create a translation file from all strings that should be translated. Use short code for target and source language (source must exist in module/lang folder',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_template_update', array(
    'long_name'   => '--temp-up',
    'description' => 'Updates a translation. Use short code for target and source language (source must exist in module/lang folder',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_vendor', array(
    'long_name'   => '--vendor',
    'description' => 'Create a translation file from all strings that should be translated. Use short code for target and source language (source must exist in module/lang folder',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_vendor_update', array(
    'long_name'   => '--vendor-up',
    'description' => 'Updates a translation. Use short code for target and source language (source must exist in module/lang folder',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_all_up', array(
    'long_name'   => '--all-up',
    'description' => 'Translate all modules / templates into a language',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_all_from_ini', array(
    'long_name'   => '--all-up-ini',
    'description' => 'Translate all modules / templates into multiple languages defined in google_translate.ini',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_vendor_from_ini', array(
    'long_name'   => '--vendor-up-ini',
    'description' => 'Translate vendor into multiple languages defined in google_translate.ini',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_all', array(
    'long_name'   => '--all-translate',
    'short_name'   => '-a',
    'description' => 'Translate a module to all known google languages with english (en_GB) as source',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_langs', array(
    'long_name'   => '--langs',
    'short_name'  => '-l',
    'description' => 'Show all available google languages. The short code should be used as target',
    'action'      => 'StoreTrue'
));

self::setOption('google_translate_collect_all', array(
    'long_name'   => '--collect-all',
    'description' => 'Will first extract new strings from all module and templates (from source). Then translate (to target). Then a new language-all will be created ',
    'action'      => 'StoreTrue'
));

self::setArgument('module',
    array('description'=> 'Specicify the module for which you will make a translation',
          'optional' => true));

self::setArgument('target',
    array('description'=> 'Specicify the target language which we will translate into',
          'optional' => true));

self::setArgument('source',
    array('description'=> 'Specicify the source language which we will translate from. If not specified en_GB will be used',
          'optional' => true));
