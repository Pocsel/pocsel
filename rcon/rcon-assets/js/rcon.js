var rconToken;
var rconMaps;
var rconRights;

function hasRight(str) {
    return rconRights.indexOf(str) >= 0;
}

$(document).ready(function() {

    // login form
    $('#login-form_submit').attr('disabled', false);
    $('#login-form_submit').click(function() {
        $('#login-form_submit').attr('disabled', true);
        $('#login-form_alert').hide('fast');
        $('#login-form_preloader').show();
        $('#navbar_exit').html('<a href="">Disconnect</a>');
        $.ajax({
            url: 'http://' + $('#login-form_host').val() + ':' + $('#login-form_port').val() + '/login',
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
                $.each(rconMaps, function() {
                    $('#maps_list').append('<tr><td>' + this.identifier + '</td><td>' + this.fullname + '</td></tr>');
                });
                $('#category-server_title').html(json.world_fullname + ' ("' + json.world_identifier + '", version ' + json.world_version + ')');
                $('#category-server').show('fast');
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
    });

    $('#ceci_test').on('shown', function (e) {
        alert(e.target);
    });

});
