// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/fileInput',   //The server endpoint we are connecting to
        data: {},

        success: function (data) {
            generate_table(data);
        },
        fail: function(error) {
            console.log("Failure");
            alert(new Error("Could not load files." + error));
            generate_table(null);
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    $('#dropdown-list').change(function() {
        var selectedVal = $(this).children('option:selected').val();
        console.log('You have chosen: ' + selectedVal + " | " +);
        

    });
});

window.addOptions = function(e) {
    if (e.value === 'test1') {
      console.log(e.value);
    } else if (e.value === 'test2') {
      console.log(e.value);
    }
}

function clear_table(table) {
    $(table).empty()
}

function generate_table(tempData) {
    const table = $('#clear-table');
    const app_table = $('#append-table');
    /*Only valid files must be displayed. Fortunately A2 loads valid SVG images*/

    if (tempData == null) {
        console.log("Null error");
        clear_table(table);
        clear_table.append('<td colspan="7" class="single-row">File not valid</td>');
    }

    const SVGImage = JSON.parse(tempData.data);

    if (SVGImage.length == 0) {
        console.log("SVG File not valid");
        clear_table(table);
        clear_table.append('<td colspan="7" class="single-row">File not valid</td>');
    } else {
        clear_table(table);
        let select = $('#dropdown-list');

        for (let i = 0; i < SVGImage.length; i++) {
            //console.log(SVGImage[i][j]);
            app_table.append(
                '<tr>' +
                '<td scope="col"><a href=\"' + SVGImage[i][0] + '\" download><img src=\"' + SVGImage[i][0] + 
                '\" alt=\"SVG Image ' + i +  '\"width="200px" height="100%"></a>' +
                '<td><a href=\"' + SVGImage[i][0] + '\" download>' + SVGImage[i][0] + '</td>' + 
                '<td>' + SVGImage[i][1] + ' KB</td>' + 
                '<td>' + SVGImage[i][2] + '</td>' + 
                '<td>' + SVGImage[i][3] + '</td>' + 
                '<td>' + SVGImage[i][4] + '</td>' + 
                '<td>' + SVGImage[i][5] + '</td>' +
                '</tr>'
            );
            /*Adding all the other options in the drop down list*/
            select.append('<option value=\"' + SVGImage[i][0] + '\">' + SVGImage[i][0] + '</option>');
        }
    }
}

function addOptions(tempImg) {
    console.log("Image value is: " + tempImg);
}