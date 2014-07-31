/*
* bizLinkToUser
*
* Usage
*
* ezAjax.bizLinkToUser.getUsers(search)
* ezAjax.bizLinkToUser.getBusinesses(search)
* ezAjax.bizLinkToUser.linkUserToBiz(search)
* ezAjax.bizLinkToUser.selectUser(search)
*
*/

ezAjax.bizLinkToUser = 
{
	selectedUser : false,
	
	/*
	* getUsers
	*
	* This functions creates the object ezAja.ajax which will be used to post the 
	* form to the server.
	* 
	* Receives a search string
	* Sends request 
	* Expects a list of users.
	* Will generate html of list of users that will be assigned to $('#users')
	* 
	*/

	getUsers : function(search)
	{
		
		if ( search.value.length <= 3 )
		{
			ezAjax.bizLinkToUser.selectedUser=false;
			$('#users').html('<span class="fade">Please type more than 3 letters</span>');
			return;
		}

		$('.biz_ticks').html('');

		// send query
		ezAjax.ajax
		( 
			'search_users', 
			{
				query: search.value
			},

			function(response)
			{
	
				if (response.error != 'OK')
				{
					alert(response.error);
				}
				else
				{

					var html = '<span class="fade">No results found for "'+search.value+'" (please keep typing)</span>';

					if ( response.users != false )
					{
						html = '';
						ezAjax.bizLinkToUser.selectedUser=false;

						for ( user in response.users )
						{
							html += '<span class="tick ticks tick'+response.users[user]['user_id']+'"><img src="/template/gfx/icons/tick_grey.gif" /></span>'
							     +'<a class="list-item" onclick="ezAjax.bizLinkToUser.selectUser('+response.users[user]['user_id']+')" style="cursor: pointer;">'
							     +response.users[user]['text']+'</a><br/>';
						}
					}

					$('#users').html(html);
				}
			},
			false,
			false
		);

	},

	/*
	* getBusinesses
	*
	* This functions creates the object ezAja.ajax which will be used to post the 
	* form to the server.
	* 
	* Receives a business search string
	* Sends request 
	* Expects a list of businesses.
	* Fenerates html of list of users that will be assigned to $('#businesses')
	* 
	*/
	
	getBusinesses : function(search)
	{

		if ( search.length <= 3 )
		{
			$('#businesses').html('<span class="fade">Please type more than 3 letters</span>');
			return;
		}
		
		ezAjax.ajax
		( 
			'search_businesses', 
			{
				query: search,
				user_id: ezAjax.bizLinkToUser.selectedUser 
			},

			function(response)
			{
				
				if (response.error != 'OK')
				{
					alert(response.error);
				}
				else
				{
					
					var html = '<span class="fade">No results found for "'+search+'" (please keep typing)</span>';
					
					if ( response.businesses != false )
					{

						html = '';
						var ticker = '';
						var title  = '';
						for ( business in response.businesses )
						{


							// If this biz is linked to another user
							if (response.businesses[business]['linkedTo'] != '' && response.businesses[business]['userId'] != ezAjax.bizLinkToUser.selectedUser )
							{
								title='Business linked to '+response.businesses[business]['linkedTo'];
								ticker='<img src="/template/gfx/icons/link.png" title="'+title+'"/>';
							}
							// If this biz is linked to me
							else if (response.businesses[business]['linkedTo'] != '' && response.businesses[business]['userId'] == ezAjax.bizLinkToUser.selectedUser)
							{
								title='Business linked to '+response.businesses[business]['linkedTo'];
								ticker='<img src="/template/gfx/icons/arrow_right.png" title="'+title+'"/>';
							}
							// If this biz lis linked to no one
							else
							{
								title='Business available for linking';
								ticker='<img src="/template/gfx/icons/tick_grey.gif" title="'+title+'"/>';
							}

							html += '<span class="tick" id="tick'+response.businesses[business]['bizId']+'" style="width:16px;" "class="biz_ticks tick'+response.businesses[business]['bizId']+'">'+ticker+'</span>'
							     + '<a title="'+title+'" class="list-item" onclick="ezAjax.bizLinkToUser.linkUserToBiz('
							     		+response.businesses[business]['bizId']+','
							     		+response.businesses[business]['userId']+','
							     		+response.businesses[business]['isLinked']+')">'
							     		+response.businesses[business]['text']
							     +'</a><br />';
						}
					}
					
					$('#businesses').html(html);

				}
				
			},
			false,
			false
		);
	},


	/*
	* linkUserToBiz
	* 
	* Parms: bizId, userId, isLinked
	* 
	* Depending on the parms passed and the value of isLinked, 
	* Posts bizId, userId an action to link, or unlink the user and business.
	* 
	* Adds or removes a tick to selected business.
	*/
	
	linkUserToBiz : function(bizId,userId,isLinked)
	{
		var message = false;
		var ticker ='';
		
		if  (!ezAjax.bizLinkToUser.selectedUser)
		{
			alert('Oops! Please find and select a user to link this business to');
			return;
		}
		
		if ( isLinked && (userId != ezAjax.bizLinkToUser.selectedUser))
		{
			alert('Oops! Business is already linked to someone else.');
			return false;
		}
		
		if (isLinked == null)
		{
			approve_or_remove = 'approve';
		}
		else if (isLinked && (userId == ezAjax.bizLinkToUser.selectedUser) )
		{
		  approve_or_remove = 'remove';
		}
		else if ( ! isLinked && (userId == ezAjax.bizLinkToUser.selectedUser))
		{
			approve_or_remove = 'approve';
		}

		ezAjax.ajax
		( 
			'biz_link_to_user',
			{
				biz_id            : bizId,
				user_id           : ezAjax.bizLinkToUser.selectedUser,
				approve_or_remove : approve_or_remove
			},

			function(response)
			{
				if (response.error != 'OK')
				{
					alert("error"+response.error);
				}
				else
				{
					if (approve_or_remove == 'approve')
					{
						ticker='<img src="/template/gfx/icons/arrow_right.png" />';
					}
					$('#tick'+bizId).html(ticker);
					
					ezAjax.bizLinkToUser.getBusinesses($('#biz_query').val());
				}
			},
			false,
			false
		);
	},

	/*
	* Adds o removes a tick to user clicked.
	*/
	
	selectUser : function(userId)
	{
		if (ezAjax.bizLinkToUser.selectedUser == userId)
		{
			ezAjax.bizLinkToUser.selectedUser=false;
			$('.ticks').html('<img src="/template/gfx/icons/tick_grey.gif" />');
			$('.biz_ticks').html('<img src="/template/gfx/icons/tick_grey.gif" />');
		}
		else
		{
			ezAjax.bizLinkToUser.selectedUser=userId;
			$('.ticks').html('');
			$('.tick'+userId).html('<img src="/template/gfx/icons/arrow_right.png" />');
		}
		
		if ( $('#biz_query').val() )
		{
			ezAjax.bizLinkToUser.getBusinesses($('#biz_query').val());
		}
		
	}

};
