/*
* Tools
*/

ezAjax.gmaps = 
{

	map : false,

	zoom : 11, // Init zoom level
	
	curLetter : 1,
	
	baseIcon : false,
	
	init : function(lat, long,zoom)
	{
		if (zoom)
		{
			ezAjax.gmaps.zoom = zoom;
		}
		
		// Create a base icon for all of our markers that specifies the
		// shadow, icon dimensions, etc.
		ezAjax.gmaps.baseIcon = new GIcon(G_DEFAULT_ICON);
		ezAjax.gmaps.baseIcon.shadow = "http://www.google.com/mapfiles/shadow50.png";
		ezAjax.gmaps.baseIcon.iconSize = new GSize(25, 29);
		ezAjax.gmaps.baseIcon.shadowSize = new GSize(1, 1);
		ezAjax.gmaps.baseIcon.iconAnchor = new GPoint(25, 29);
		ezAjax.gmaps.baseIcon.infoWindowAnchor = new GPoint(9, 2);
		
		ezAjax.gmaps.map = new GMap2(document.getElementById("map"));
		var mapControl = new GMapTypeControl();
		ezAjax.gmaps.map.addControl(mapControl);
		ezAjax.gmaps.map.addControl(new GLargeMapControl());
		var point = new GLatLng(lat, long);
		ezAjax.gmaps.map.setCenter(point, ezAjax.gmaps.zoom);
	},

	addMarker : function(lat, long, html, idx, doTimeout)
	{

//		var letter = String.fromCharCode("A".charCodeAt(0) + ezAjax.gmaps.curLetter);
		var letteredIcon = new GIcon(ezAjax.gmaps.baseIcon);
		letteredIcon.image = "/template/gfx/pins/f" + ezAjax.gmaps.curLetter + ".gif";
		ezAjax.gmaps.curLetter++;
		if ( ezAjax.gmaps.curLetter == 99 ) ezAjax.gmaps.curLetter = 1;
		
		point = new GLatLng(lat, long);
		var marker = new GMarker(point,{ icon:letteredIcon });
		ezAjax.gmaps.map.addOverlay(marker);
		GEvent.addListener(marker, "click", function() {marker.openInfoWindowHtml(html);});
		
		// Bind this action to marker link/img
		$(document).ready(function()
		{
			$(".marker"+idx).bind("click", function()
			{
				if (ezAjax.gmaps.zoom!=14)
				{
					ezAjax.gmaps.setZoom(14);
				}
				marker.openInfoWindowHtml(html);
      		
    		});
		});
		
		if ( doTimeout === true )
		{
			setTimeout(function(){marker.openInfoWindowHtml(html)},250);
		}
	},

	setZoom : function(level)
	{
		ezAjax.gmaps.zoom = level;
		ezAjax.gmaps.map.setZoom(level);
	}

};
