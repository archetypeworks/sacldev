var acc = document.getElementsByClassName("accordion");
var i;

for (i = 0; i < acc.length; i++) {
    acc[i].onclick = function(){
        this.classList.toggle("active");
        this.nextElementSibling.classList.toggle("show");
 	};
}
jQuery(document).ready(function(){
	jQuery("#block-menu-menu-left-sidebar").find("a.active-trail").parent().addClass( "active" );
	jQuery("#block-menu-menu-left-sidebar").find("a.active").parent().addClass( "active" );	
	jQuery("#block-menu-menu-left-sidebar").find("li.active").parent().addClass( "show" );
	jQuery("#block-menu-menu-left-sidebar").find("li.active").next().each(function(){
		if(jQuery(this).is("ul")){
			jQuery(this).addClass( "show" );
		}
	})
});



jQuery(document).ready(function(){
jQuery('#block-menu-menu-left-sidebar > div > ul > li.accordion:not(.active)').click();
	/************************* start hide custom title from page*****************************/
	jQuery('.content > div.field.field-name-body > .field-items > .field-item').find('*:first').each(function(){
	if(jQuery(this).is("h3")){
	 jQuery(this).hide();
	 clearInterval(myVar);
	 }
	})
	/************************* End hide custom title from page*****************************/
});

var myVar=setInterval(function(){ 
	if(jQuery('.content > div.field.field-name-body > .field-items > .field-item').length>0)
		jQuery('.content > div.field.field-name-body > .field-items > .field-item').find('*:first').each(function(){
		if(jQuery(this).is("h3"))
		 jQuery(this).hide();
		})
 }, 300);
 ;
