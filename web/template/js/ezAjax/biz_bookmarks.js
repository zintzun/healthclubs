/*
* bizBookmarks
*
* Usage
*
* ezAjax.bizBookmarks.addBookmark(userId,bizId)
*
*/

ezAjax.bizBookmarks = 
{
	/*
	* addBookmark
	*
	* This functions creates the object ezAja.ajax which will be used to post the 
	* form to the server.
	* 
	* Receives  userId and bizId
	* Sends request 
	* Expects confirmation of biz added to bookmarks or if biz already in bookmarks.
	* Alert user with reponse.
	* 
	*/

	addBookmark : function(userId,bizId)
	{

		$('#biz_bookmark').html('<img src="/template/gfx/loading-small.gif" style="vertical-align: middle;"/>');

		ezAjax.ajax
		( 
			'biz_bookmark',
			{
				user_id           : userId,
				biz_id            : bizId
			},

			function(response)
			{
				if (response.error != 'OK')
				{
					alert("error"+response.error);
				}
				else
				{
					$('#biz_bookmark').html('');
					
					if ( response.isOldBookmark )
					{
						alert("This business is already in your bookmarks!");
					}
					else{
						alert("We added this business to your bookmarks!");
					}
				}
			},
			false,
			false
		);
	}

};
