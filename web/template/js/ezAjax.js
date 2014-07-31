var includes =
[
	'ezAjax.js',
	'search.js',
	'tools.js',
	'gmaps.js',
	'register.js',
	'review.js',
	'biz_link_to_user.js',
	'biz_bookmarks.js',
	'biz_leads.js',
	'biz_listings.js',
	'photos.js'
];

var time = new Date();

for ( var i = 0; i < includes.length; i++ ) 
{
	document.write( '<script type="text/javascript" src="/template/js/ezAjax/'+ includes[i]+'"></script>' );
}
