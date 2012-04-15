var rconToken;
var rconMaps;

$(document).ready(function() {

    // login form
    $('#login-form_submit').attr('disabled', false);
    $('#login-form_submit').click(function() {
        $('#login-form_submit').attr('disabled', true);
        $('#login-form_alert').hide('fast');
        $('#navbar_exit').html('<a href="">Disconnect</a>');
        $.ajax({
            url: 'http://' + $('#login-form_host').val() + ':' + $('#login-form_port').val() + '/login',
            type: 'POST',
            success: function(json) {
                $('#login-form').hide('fast');
                $('#navbar_login').html('<a href="#">Connected as ' + $('#login-form_login').val() + '</a>');
                json = $.parseJSON(json);
                rconToken = json.token;
                rconMaps = json.maps;
                $.each(rconMaps, function() {
                    $('#maps_list').append('<tr><td>' + this.identifier + '</td><td>' + this.fullname + '</td></tr>');
                });
                $('#server').show('fast');
            },
            error: function(json, errorType, httpError) {
                $('#login-form_alert').html('Failed to login. ' + httpError);
                $('#login-form_alert').show('fast');
                $('#login-form_submit').attr('disabled', false);
                $('#navbar_exit').html('');
            }
        });
    });

});
