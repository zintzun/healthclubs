/*
* Search
*/

ezAjax.search = 
{

	suggest : function(q)
	{
		if ( q.value.length >= 3 )
		{
			ezAjax.search.getSuggestions(q.value);
		}
		else
		{
			$('#suggestions').fadeOut('fast');
		}
	},
	
	getSuggestions : function(query)
	{

		ezAjax.ajax
		( 
			'suggestions_get', 
			{
				query: query
			},

			function(response)
			{				
				if ( response.error != 'OK' )
				{
					alert(response.error);
				}
				else if ( response.suggestions != false )
				{
					var html = '';
					for ( prop in response.suggestions )
					{
						html += '<a href="'+response.suggestions[prop]['link']+'">'+response.suggestions[prop]['text']+'</a><br />';
					}
					$('#suggestions').html(html);
					$('#suggestions').fadeIn('fast').animate({scrollTop: 0}, 500);
					$("#query").bind('blur',function(){$('#suggestions').fadeOut('fast');});			
				}
			},
			false,
			false
		);

	},
	
	searchByZip : function(e)
	{
		
		if ( ezAjax.tools.isFormTrigger(e) !== true )
		{
			return;
		}

		// This gets it from the home page
		var searchType = $('input[name=biz_type]:checked').val();
		var zip        = $('#zip').val();
		
		if ( searchType === undefined )
		{
			// This gets it from any page other than home
			searchType = $('input[name=biz_type]').val();
		}

		if ( zip == '' || zip == 'Enter Zip Code' || ! zip.match(/\d\d\d\d\d/) )
		{
			return;
		}

		location.href = '/'+searchType+'/by-zip/'+zip;

	}
};