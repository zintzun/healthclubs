/*
* Search
*/

ezAjax.photos = 
{

	selectStocPick : function(imgObj)
	{
		$('#stock-pic').val(imgObj.src);
		$('#current-pic').html('<img src="'+imgObj.src+'" />');
	},
	
	unselectStocPick : function()
	{
		$('#stock-pic').val('');
		$('#current-pic').html('<img src="/template/gfx/no-image.jpg" />');
	}
};