// $(document).ready(function(){
//     $("#clock").click(function(){
//         $(this).css("color","red");
//     });
// });
    
    function realtimeClock() {
        var d = new Date();
        var h = d.getHours();
        var m = d.getMinutes();
        var s = d.getSeconds();

        var amPm = (h > 12) ? "PM" : "AM";

        h = (h>12) ? (h-12) : h;

        h = ("0" + h).slice(-2);
        m = ("0" + m).slice(-2);
        s = ("0" + s).slice(-2);

        document.getElementById("clock").innerHTML =
        h + " : "+  m+" : "+ s + " : " + amPm ;

        var l = setTimeout(realtimeClock,500);
    }




    
    // SMOOTH SCROLLING PLUS OFFSET FOR FIXED NAV

$('a[href*="#"]')
// Remove links that don't actually link to anything
.not('[href="#"]')
.not('[href="#0"]')
.on('click', function(event) {   

    // Make sure this.hash has a value before overriding default behavior
    if (this.hash !== "") {

        // Store hash
        var hash = this.hash;

        // Using jQuery's animate() method to add smooth page scroll
        // The optional number (800) specifies the number of milliseconds it takes to scroll to the specified area
        // - 70 is the offset/top margin
        $('html, body').animate({
            // scrollTop: $(hash).offset().top - 70
            scrollTop: $(hash).offset().top - 3.875
        }, 800, function() {

            // Add hash (#) to URL when done scrolling (default click behavior), without jumping to hash
            if (history.pushState) {
                history.pushState(null, null, hash); 
            } else {
                window.location.hash = hash;
            } 
        });
        return false;    
    } // End if
});
