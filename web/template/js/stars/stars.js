/*
* Creates a set of div elements with a star as a background
*
* divId: is the Id of the container for the stars
* value: number of stars to be highlighted
*/
function doBuildStars(divId, value)
{
	value = (value === undefined)? 0: value;
	$('#'+divId).html(createStars(divId));

	bindStarsActions(divId,value);
}

/*
* Creates an set of div elements with a star as a background
* divId: is the Id of the container for the stars
* value: number of stars to be highlighted
*/
function createStars(divId, value)
{
	value = (value == null)? 0: value;
	var html='<input type="hidden" id="value-'+divId+'" value="'+value+'" />'+"\n";
		
	for(var i=1;i<=5;i++)
	{
		html += '<div id="'+divId+'-'+i+'" class="my-stars star-grey '+(i <= value? 'star-red':'')+'"></div>'+"\n";
	}
	return(html);
}

/*
* Given an element id (divId), all the elements with class ".my-stars" will be binded to 
* events: mousover, mouseout and click.
*/
function bindStarsActions(divId)
{
	var selector ='#'+divId+' .my-stars';

	// Hightlight stars (Yellow color) on hover over
	$(selector).bind('mouseover',function()
	{
		highlightStars(divId, this.id.match(/\d$/)[0],'star-red', 'star-yellow');
	});
	
	// Reset stars to previous color before hover over. If a star was clicked set red stars.
	$(selector).bind('mouseout',function()
	{
		highlightStars(divId, $('#value-'+divId).val(), 'star-yellow', 'star-red');
	});
	
	// Set the valueReset the mouse out value (grey out stars)
	$(selector).bind('click',function()
	{
		$('#value-'+divId).val(this.id.match(/\d$/)[0]);
		highlightStars(divId, this.id.match(/\d$/)[0], 'star-yellow', 'star-red');
	});

}


/*
* Stars (divs) are "highlighted" by adding a class which changes the 
* background image.
*
* Highlights all stars <= curStar with newColorClass
*/
function highlightStars(divId, curStar, oldColorClass, newColorClass)
{
	$('#'+divId+' .my-stars').each(function()
	{
		$(this).removeClass(oldColorClass);
		if ( this.id.match(/\d$/)[0] <= curStar )
		{
			$(this).addClass(newColorClass);
		}
	});
}


