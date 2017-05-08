jQuery(document).ready(function(){
if(jQuery('#edit-commerce-tax-certificate-tax-exempt-in-the-state-of-texas-check').is(":checked"))
	  jQuery('[for="edit-commerce-tax-certificate-tax-exempt-in-the-state-of-texas-check"]').css("font-weight","bold");
	

	jQuery('#edit-commerce-tax-certificate-tax-exempt-in-the-state-of-texas-check').change(function(){
	if(jQuery(this).is(":checked"))
	  jQuery('[for="edit-commerce-tax-certificate-tax-exempt-in-the-state-of-texas-check"]').css("font-weight","bold");
	else
	  jQuery('[for="edit-commerce-tax-certificate-tax-exempt-in-the-state-of-texas-check"]').css("font-weight","normal");

	});
	jQuery( document ).ajaxStop(function() {
	   afterajaxcomplete_commerce_tax_certificate();
	});	

});
function afterajaxcomplete_commerce_tax_certificate(){

jQuery('.submit-save').show();
	if(jQuery('[name="commerce_tax_certificate[tax_certificate][fid]"]').val()>0)
	jQuery('.submit-save').text('Attach Certificate');
	if(jQuery('[name="commerce_tax_certificate[tax_certificate][fid]"]').val()==0)
	jQuery('.submit-save').text('Detach Certificate');
	

}

