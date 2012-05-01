var generatedIdCount = 0;
var logScrollToBottom = true;
var rconToken;
var rconMaps;
var rconRights;
var rconPlugins;
var rconUrl;
var rconEntityFiles;
var rconEntityTypes;
var rconDebug;
var currentMap;

/*
 * On document ready
 */
$(document).ready(function() {

    // "Edit lua file" popup configuration
    $('#lua-modal').modal({ keyboard: true, show: false, backdrop: true });
    $('#lua-modal_ok').click(function() {
        $('#lua-modal_log').hide('fast');
        $('#lua-modal_text').show('fast');
        $('#lua-modal_swap').show('fast');
        $('#lua-modal_ok').hide('fast');
    });

    // Login form
    $('#login-form_submit').attr('disabled', false);
    $('#login-form').submit(function() {
        $('#login-form_submit').attr('disabled', true);
        $('#login-form_alert').hide('fast');
        $('#login-form_preloader').show();
        $('#navbar_exit').html('<a href="">Disconnect</a>');
        rconUrl = 'http://' + $('#login-form_host').val() + ':' + $('#login-form_port').val() + '/';
        $.ajax({
            url: rconUrl + 'login',
            type: 'POST',
            timeout: 5000,
            data: {
                login: $('#login-form_login').val(),
                password: $('#login-form_password').val()
            },
            dataType: 'json',
            success: function(json) {
                $('#login-form').hide('fast');
                $('#navbar_login').html('<a href="#">Connected as ' + $('#login-form_login').val() + '</a>');
                rconToken = json.token;
                rconMaps = json.maps;
                rconRights = json.rights;
                rconPlugins = json.plugins;
                rconEntityFiles = json.entity_files;
                rconEntityTypes = json.entity_types;
                rconDebug = json.debug;
                $('#category-server_title').html(json.world_fullname +
                    ' ("' + json.world_identifier + '", version ' + json.world_version + ', debugging ' + (rconDebug ? 'on' : 'off') + ')');
                fillStaticTables();
                setTimeout(startFetchers, 250);
            },
            error: function(json, errorType, httpError) {
                $('#login-form_alert').html('Failed to login.');
                if (httpError)
                    $('#login-form_alert').append('<br />Error: ' + httpError + '.');
                $('#login-form_alert').show('fast');
                $('#login-form_submit').attr('disabled', false);
                $('#navbar_exit').html('');
                $('#login-form_preloader').hide();
            }
        });
        return false;
    });

    // Execute form
    $('#execute_submit').click(function() {
        $('#execute_submit').hide();
        $('#execute_alert').hide('fast');
        $('#execute_preloader').show('fast');
        $.ajax({
            url: rconUrl + 'map/' + currentMap + '/execute/' + $('#execute_plugin').val(),
            type: 'POST',
            timeout: 5000,
            data: {
                lua: $('#execute_text').val()
            },
            headers: {
                'Rcon-Token': rconToken
            },
            dataType: 'json',
            success: function(json) {
                $('#execute_submit').show('fast');
                $('#execute_preloader').hide('fast');
                if (json.log) {
                    $('#execute_alert').html(json.log);
                    $('#execute_alert').show('fast');
                }
            },
            error: function() {
                alert("Failed to execute script.");
                $('#execute_submit').show('fast');
                $('#execute_preloader').hide('fast');
            }
        });
    });

    // logs tabs
    $('#logs_all-tab').click(function() {
        $('#logs_textareas > textarea').hide();
        $('#logs_all').show();
    });
    $('#logs_error-tab').click(function() {
        $('#logs_textareas > textarea').hide();
        $('#logs_error').show();
    });

    // logs scroll to bottom checkbox
    $('#logs_scroll').click(function() {
        logScrollToBottom = !logScrollToBottom;
    });

});

/*
 * Utility functions
 */
function hasRight(str) {
    return rconRights.indexOf(str) >= 0;
}

function generateId(str) {
    generatedIdCount++;
    return str + "_" + generatedIdCount;
}

function scrollToBottom(textarea) {
    textarea.scrollTop(textarea[0].scrollHeight - textarea.height());
}

/*
 * Map switching (tabs)
 */
function activateMap(map) {
    currentMap = map;
    $('#execute_submit').html('Run script on ' + currentMap);
    $('#execute_alert').hide('fast');
    // TODO clear les groupes de données de la map
}

/*
 * Initial stuff done on successful login
 */
function fillStaticTables() {
    // "world" category
    $.each(rconMaps, function() {
        $('#maps_list').append('<tr><td>' + this.identifier + '</td><td>' + this.fullname + '</td></tr>');
    });
    $.each(rconPlugins, function() {
        $('#plugins_list').append('<tr><td>' + this.identifier + '</td><td>' + this.fullname + '</td></tr>');
    });
    $('#category-server').show('fast');

    // maps category
    for (var i = 0; i < rconMaps.length; i++) {
        var id = generateId("map_tab");
        if (!i) {
            $('#map_tabs').append('<li class="active"><a id ="' + id + '" data-toggle="tab">' + rconMaps[i].identifier + '</a></li>');
            activateMap(rconMaps[i].identifier);
        }
        else
            $('#map_tabs').append('<li><a id="' + id + '" data-toggle="tab">' + rconMaps[i].identifier + '</a></li>');
        $('#' + id).click({ map: rconMaps[i].identifier }, function (e) {
            activateMap(e.data.map);
        });
    }
    $.each(rconPlugins, function() {
        $('#execute_plugin').append('<option>' + this.identifier + '</option>');
    });
    $('#category-maps').show('fast');

    // entities category
    if (rconDebug) {
        $.each(rconEntityFiles, function() {
            if (hasRight("execute")) {
                var id = generateId("entity-files_edit");
                $('#entity-files_list').append('<tr><td>' + this.plugin + '</td><td>' + this.file + '</td><td><a id="' + id + '">Edit</a></td></tr>');
                $('#' + id).click({ plugin: this.plugin, file: this.file }, function(e) {
                    editLuaFile(e.data.plugin, e.data.file);
                });
            }
            else
                $('#entity-files_list').append('<tr><td>' + this.plugin + '</td><td>' + this.file + '</td><td>-</td></tr>');
        });
        $('#entity-files_group').show();
    }
    $.each(rconEntityTypes, function() {
        $('#entity-type_list').append('<tr><td>' + this.plugin + '</td><td>' + this.name + '</td><td>' + (this.positional ? '<i class="icon-ok"></i>' : '') + '</td></tr>');
    });
    $('#category-entities').show('fast');
}

/*
 * Lua file popup when "edit" is clicked
 */
function editLuaFile(plugin, file) {
    $('#lua-modal_title').html(plugin + '/' + file);
    $('#lua-modal_preloader').show();
    $('#lua-modal_text').hide();
    $('#lua-modal_swap').hide();
    $('#lua-modal_ok').hide();
    $('#lua-modal_log').hide();
    $('#lua-modal').modal('show');
    $.ajax({
        url: rconUrl + 'entity_file/' + plugin + '/' + file,
        timeout: 5000,
        headers: {
            'Rcon-Token': rconToken
        },
        dataType: 'json',
        success: function(json) {
            $('#lua-modal_text').val(json.lua);
            $('#lua-modal_preloader').hide('fast');
            $('#lua-modal_text').show('fast');
            $('#lua-modal_swap').show('fast');
        },
        error: function() {
            alert("Failed to fetch entity file.");
            $('#lua-modal').modal('hide');
        }
    });
    $('#lua-modal_swap').off('click');
    $('#lua-modal_swap').click({ plugin: plugin, file: file }, function(e) {
        $('#lua-modal_swap').hide('fast');
        $('#lua-modal_ok').show('fast');
        $('#lua-modal_log').empty();
        $('#lua-modal_log').show('fast');
        $('#lua-modal_text').hide('fast');
        for (var i = 0; i < rconMaps.length; i++) {
            map = rconMaps[i].identifier;
            $.ajax({
                url: rconUrl + 'map/' + map + '/execute/' + e.data.plugin,
                type: 'POST',
                timeout: 5000,
                data: {
                    lua: $('#lua-modal_text').val()
                },
                headers: {
                    'Rcon-Token': rconToken
                },
                dataType: 'json',
                success: function(json) {
                    if (json.log)
                        $('#lua-modal_log').append('<div id="execute_alert" class="alert alert-error">' + map + ': ' + json.log + '</div>');
                    else
                        $('#lua-modal_log').append('<div id="execute_alert" class="alert alert-success">' + map + ': OK.</div>');
                },
                error: function() {
                    $('#lua-modal_log').append('<div id="execute_alert" class="alert alert-error">' + map + ': Failed to send script.</div>');
                }
            });
        }
        $.ajax({
            url: rconUrl + 'entity_file/' + e.data.plugin + '/' + e.data.file,
            type: 'POST',
            timeout: 5000,
            data: {
                lua: $('#lua-modal_text').val()
            },
            headers: {
                'Rcon-Token': rconToken
            }
        });
    });
}

/*
 * First setTimeouts to start fetching data regularly
 */
function startFetchers() {
    if (hasRight('logs')) {
        $('#category-logs').show('fast');
        setTimeout(fetchLogs, 1000);
    }
    if (hasRight('entities')) {
        $('#entities_group').show('fast');
        setTimeout(fetchEntities, 1500);
    }
    if (hasRight('messages')) {
        $('#messages_group').show('fast');
        setTimeout(fetchMessages, 2000);
    }
    if (hasRight('rcon_sessions')) {
        $('#rcon-sessions_group').show('fast');
        setTimeout(fetchRconSessions, 2500);
    }
    if (hasRight('execute')) {
        $('#execute_group').show('fast');
    }
    // Logs 'Load' tab (only in debug)
    if (rconDebug) {
        $('#logs_tabs').append('<li><a id="logs_load-tab" data-toggle="tab">Load</a></li>');
        $('#logs_load-tab').click(function () {
            $('#logs_textareas > textarea').hide();
            $('#logs_load').show();
        });
        setTimeout(fetchLoadLog, 3000);
    }
}

/*
 * Fetches the load log
 */
function fetchLoadLog() {
    $.ajax({
        url: rconUrl + 'load_log',
        headers: {
            'Rcon-Token': rconToken
        },
        dataType: 'json',
        success: function(json) {
            if (logScrollToBottom) {
                $('#logs_load').append(json.log);
                scrollToBottom($('#logs_load'));
            }
            else {
                var pos = $('#logs_load').scrollTop();
                $('#logs_load').append(json.log);
                $('#logs_load').scrollTop(pos);
            }
        },
        error: function() {
            setTimeout(fetchLoadLog, 20000);
        }
    });
}

/*
 * Fetches log messages regularly
 */
function fetchLogs() {
    $.ajax({
        url: rconUrl + 'logs',
        headers: {
            'Rcon-Token': rconToken
        },
        dataType: 'json',
        success: function(json) {
            if (logScrollToBottom) {
                $('#logs_all').append(json.normal);
                scrollToBottom($('#logs_all'));
                $('#logs_error').append(json.error);
                scrollToBottom($('#logs_error'));
            }
            else {
                var pos;
                pos = $('#logs_all').scrollTop();
                $('#logs_all').append(json.normal);
                $('#logs_all').scrollTop(pos);
                pos = $('#logs_error').scrollTop();
                $('#logs_error').append(json.error);
                $('#logs_error').scrollTop(pos);
            }
            setTimeout(fetchLogs, 500);
        },
        error: function() {
            setTimeout(fetchLogs, 20000);
        }
    });
}

/*
 * Fetches entity instances regularly
 */
function fetchEntities() {
    $.ajax({
        url: rconUrl + 'map/' + currentMap + '/entities',
        headers: {
            'Rcon-Token': rconToken
        },
        dataType: 'json',
        success: function(json) {
            $('#entities_list').empty();
            $.each(json, function() {
                $('#entities_list').append('<tr><td>' + this.id + '</td><td>' + this.plugin + '</td><td>' + this.type + '</td><td>' + (this.positional ? '<i class="icon-ok"></i>' : '') + '</td><td>-</td></tr>');
            });
            setTimeout(fetchEntities, 2500);
        },
        error: function() {
            setTimeout(fetchEntities, 20000);
        }
    });
}

/*
 * Fetches pending messages regularly
 */
function fetchMessages() {
}

/*
 * Fetches rcon sessions regularly
 */
function fetchRconSessions() {
    $.ajax({
        url: rconUrl + 'rcon_sessions',
        headers: {
            'Rcon-Token': rconToken
        },
        dataType: 'json',
        success: function(json) {
            $('#rcon-sessions_list').empty();
            $.each(json, function() {
                $('#rcon-sessions_list').append('<tr><td>' + this.login + '</td><td>' + this.user_agent + '</td></tr>');
            });
            setTimeout(fetchRconSessions, 10000);
        },
        error: function() {
            setTimeout(fetchRconSessions, 20000);
        }
    });
}
