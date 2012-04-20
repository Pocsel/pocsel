var generatedIdCount = 0;
var rconToken;
var rconMaps;
var rconRights;
var rconPlugins;
var rconUrl;
var rconEntityFiles;
var currentMap;

$(document).ready(function() {

    $('#lua-modal').modal({ keyboard: true, show: false, backdrop: true });

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
            success: function(json) {
                $('#login-form').hide('fast');
                $('#navbar_login').html('<a href="#">Connected as ' + $('#login-form_login').val() + '</a>');
                json = $.parseJSON(json);
                rconToken = json.token;
                rconMaps = json.maps;
                rconRights = json.rights;
                rconPlugins = json.plugins;
                rconEntityFiles = json.entity_files;
                fillStaticTables();
                $('#category-server_title').html(json.world_fullname + ' ("' + json.world_identifier + '", version ' + json.world_version + ')');
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
            success: function(json) {
                json = $.parseJSON(json);
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

    $('#lua-modal_ok').click(function() {
        $('#lua-modal_log').hide('fast');
        $('#lua-modal_text').show('fast');
        $('#lua-modal_swap').show('fast');
        $('#lua-modal_ok').hide('fast');
    });

});

function hasRight(str) {
    return rconRights.indexOf(str) >= 0;
}

function generateId(str) {
    generatedIdCount++;
    return str + "_" + generatedIdCount;
}

function activateMap(map) {
    currentMap = map;
    $('#execute_submit').html('Run script on ' + currentMap);
    $('#execute_alert').hide('fast');
    // TODO clear les groupes de données de la map
}

function fillStaticTables() {
    $.each(rconMaps, function() {
        $('#maps_list').append('<tr><td>' + this.identifier + '</td><td>' + this.fullname + '</td></tr>');
    });
    $.each(rconPlugins, function() {
        $('#plugins_list').append('<tr><td>' + this.identifier + '</td><td>' + this.fullname + '</td></tr>');
    });
    $('#category-server').show('fast');

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
    $('#category-maps').show('fast');

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
    $.each(rconPlugins, function() {
        $('#execute_plugin').append('<option>' + this.identifier + '</option>');
    });
    $('#category-entities').show('fast');
}

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
        success: function(json) {
            json = $.parseJSON(json);
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
                success: function(json) {
                    json = $.parseJSON(json);
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
}

function fetchLogs() {
}

function fetchEntities() {
}

function fetchMessages() {
}

function fetchRconSessions() {
    $.ajax({
        url: rconUrl + 'rcon_sessions',
        headers: {
            'Rcon-Token': rconToken
        },
        success: function(json) {
            json = $.parseJSON(json);
            $('#rcon-sessions_list').empty();
            $.each(json, function() {
                $('#rcon-sessions_list').append('<tr><td>' + this.login + '</td><td>' + this.user_agent + '</td></tr>');
            });
            setTimeout(fetchRconSessions, 10000);
            $('#rcon-sessions_list').hide();
            $('#rcon-sessions_list').show();
        },
        error: function() {
            setTimeout(fetchRconSessions, 20000);
        }
    });
}
