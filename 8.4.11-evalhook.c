#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_compile.h"
#include "php_evalhook.h"

static zend_op_array *(*orig_compile_string)(zend_string *source_string, const char *filename, zend_compile_position position);
static zend_bool evalhook_hooked = 0;

static zend_op_array *evalhook_compile_string(zend_string *source_string, const char *filename, zend_compile_position position)
{
    int c;
    int allow = 0, tofile = 0;
    char *copy;

    if (ZSTR_LEN(source_string) == 0) {
        return orig_compile_string(source_string, filename, position);
    }

    copy = estrndup(ZSTR_VAL(source_string), ZSTR_LEN(source_string));

    php_printf("Script tries to evaluate the following string:\n");
    php_printf("----\n%s\n----\n", copy);
    php_printf("Allow execution? [y/N/o=output.php]\n");

    while ((c = getchar()) != '\n' && c != EOF) {
        if (c == 'y' || c == 'Y') {
            allow = 1;
        } else if (c == 'o' || c == 'O') {
            tofile = 1;
        }
    }

    if (tofile) {
        FILE *f = fopen("output.php", "a");
        if (f) {
            fprintf(f, "<?php\n%s\n?>\n", copy);
            fclose(f);
            php_printf("evalhook: dumped to output.php\n");
        } else {
            php_printf("evalhook: failed to open output.php\n");
        }
        efree(copy);
        return NULL; // dump only, tidak dieksekusi
    }

    efree(copy);

    if (allow) {
        return orig_compile_string(source_string, filename, position);
    }

    zend_error(E_ERROR, "evalhook: script abort due to disallowed eval()");
    return NULL;
}

PHP_MINIT_FUNCTION(evalhook)
{
    if (!evalhook_hooked) {
        evalhook_hooked = 1;
        orig_compile_string = zend_compile_string;
        zend_compile_string = evalhook_compile_string;
    }
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(evalhook)
{
    if (evalhook_hooked) {
        evalhook_hooked = 0;
        zend_compile_string = orig_compile_string;
    }
    return SUCCESS;
}

PHP_MINFO_FUNCTION(evalhook)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "evalhook support", "enabled");
    php_info_print_table_end();
}

zend_module_entry evalhook_module_entry = {
    STANDARD_MODULE_HEADER,
    "evalhook",
    NULL,
    PHP_MINIT(evalhook),
    PHP_MSHUTDOWN(evalhook),
    NULL,
    NULL,
    PHP_MINFO(evalhook),
    "0.3",
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_EVALHOOK
ZEND_GET_MODULE(evalhook)
#endif
