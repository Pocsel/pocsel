var generatedIdCount = 0;
var rconToken;
var rconMaps;
var rconRights;
var rconPlugins;
var rconUrl;
var rconEntityFiles;

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

});

function hasRight(str) {
    return rconRights.indexOf(str) >= 0;
}

function generateId(str) {
    generatedIdCount++;
    return str + "_" + generatedIdCount;
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
        console.log('map ' + i + ': ' + rconMaps[i].identifier);
        if (i == 0)
            $('#map_tabs').append('<li class="active"><a data-toggle="tab">' + rconMaps[i].identifier + '</a></li>');
        else
            $('#map_tabs').append('<li><a data-toggle="tab">' + rconMaps[i].identifier + '</a></li>');
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
    $('#category-entities').show('fast');
}

function editLuaFile(plugin, file) {
    $('#lua-modal_title').html(plugin + '/' + file);
    $('#lua-modal_preloader').show();
    $('#lua-modal_text').hide();
    $('#lua-modal_swap').hide();
    $('#lua-modal').modal('show');
    $.ajax({
        url: rconUrl + 'entity_file/' + plugin + '/' + file,
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
        }
    });
    $('#lua-modal_swap').click({ plugin: plugin }, function(e) {
        $('#lua-modal_swap').hide();
        $('#lua-modal_preloader').show('fast');
        $.ajax({
            url: rconUrl + 'execute/' + e.data.plugin,
            type: 'POST',
            data: {
                lua: $('#lua-modal_text').val()
            },
            headers: {
                'Rcon-Token': rconToken
            },
            success: function(json) {
                $('#lua-modal').modal('hide');
            },
            error: function() {
                alert("Failed to swap entity file.");
                $('#lua-modal_swap').show('fast');
                $('#lua-modal_preloader').hide('fast');
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
        error: function(json, errorType, httpError) {
            setTimeout(fetchRconSessions, 20000);
        }
    });
}
