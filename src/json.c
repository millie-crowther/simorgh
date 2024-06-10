#include "json.h"

#include <regex.h>
#include <string.h>

typedef enum json_token_type_t {
    JSON_TOKEN_TYPE_NULL,
    JSON_TOKEN_TYPE_FALSE,
    JSON_TOKEN_TYPE_TRUE,
    JSON_TOKEN_TYPE_NUMBER,
    JSON_TOKEN_TYPE_STRING,
    JSON_TOKEN_TYPE_COLON,
    JSON_TOKEN_TYPE_COMMA,
    JSON_TOKEN_TYPE_OPEN_BRACE,
    JSON_TOKEN_TYPE_CLOSE_BRACE,
    JSON_TOKEN_TYPE_OPEN_BRACKET,
    JSON_TOKEN_TYPE_CLOSE_BRACKET,
    JSON_TOKEN_TYPE_WHITESPACE,
    JSON_TOKEN_TYPE_INVALID
} json_token_type_t;

typedef struct json_token_t {
    json_token_type_t type;
    const char * string;
} json_token_t;

typedef struct json_token_regex_t {
    json_token_type_t type;
    const char * regex_string;
    regex_t regex
} json_token_regex_t;

json_token_regex_t regexes[JSON_TOKEN_TYPE_INVALID] = {
    { .type = JSON_TOKEN_TYPE_TRUE, .regex_string = "^true" },
    { .type = JSON_TOKEN_TYPE_INVALID }
};

bool json_compile_regular_expressions(){
    for (uint32_t i = 0; regexes[i].type != JSON_TOKEN_TYPE_INVALID; i++){
        int exit_code = regcomp(&regexes[i].regex, regexes[i].regex_string, REG_EXTENDED);
        if (exit_code != 0){
            return false;
        }
    }

    return true;
}

bool tokenize(const char * string, json_token_t * tokens){
    while (strlen(string) > 0){
        for (uint32_t i = 0;; i++){
            if (regexes[i].type == JSON_TOKEN_TYPE_INVALID){
                return false;
            }
            
            regmatch_t match;
            int result = regexec(&regexes[i].regex, string, 1, &match, 0);
            if (result == 0){
                if (regexes[i].type != JSON_TOKEN_TYPE_WHITESPACE){
                    *tokens = (json_token_t){
                        .type = regexes[i].type,
                        .string = string,
                    };
                    tokens++;
                }
                string += match.rm_eo;
                break;
            }
        }
    }

    return true;
}

uint64_t hash(const char * string) {
    // see: http://www.cs.yorku.ca/~oz/hash.html
    uint64_t hash = 5381;

    for (; *string != '\0'; string++){
        hash = ((hash << 5) + hash) + *string;
    }

    return hash;
}

json_value_t * json_object_get(const json_object_t * object, const char * string){
    uint64_t index = hash(string) % JSON_OBJECT_BUCKETS_COUNT;
    json_object_bucket_t bucket = object->buckets[index];
    for (uint32_t i = 0; i < bucket.length; i++){
        if (strcmp(string, bucket.key_pairs[i].key) == 0){
            return key_pair.value;
        }
    }
    return nullptr;
}
