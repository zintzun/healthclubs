/*
* bizLeads
*
* Usage
*
* ezAjax.bizLeads.toggleBizLeadStatus(leadId)
*
*/

ezAjax.bizLeads = 
{
	/*
	* toggleLeadStatus
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
	toggleLeadStatus : function(leadId)
	{
		html = $('#tick'+leadId).html();
		$('#tick'+leadId).html('<img src="/template/gfx/icons/tick.png" class="tick" title="Mark as complete">');
		
		ezAjax.ajax
		( 
			'biz_lead',
			{
				lead_id: leadId
			},

			function(response)
			{
				if (response.error != 'OK')
				{
					alert("error"+response.error);
					$('#tick'+leadId).html(html);
				}
				else
				{
					if (response.isDone == '1')
					{
						$('#tick'+leadId).html('<img src="/template/gfx/icons/tick.png" class="tick" >');
					}
				}
			},
			false,
			false
		);
	},
	
	toggleCreditLead : function(leadId)
	{
		// Take note of orig link text
		var origText = $('#credit-link-'+leadId).text();
		var newText  = 'UNCREDIT LEAD';
		
		if ( origText == 'UNCREDIT LEAD' )
		{
			newText = 'credit lead';
		}

		// Now toggle the link to the other state		
		$('#credit-link-'+leadId).text(newText);
	
		ezAjax.ajax
		( 
			'biz_credit_lead',
			{
				lead_id: leadId
			},

			function(response)
			{
				if (response.error != 'OK')
				{
					alert("error"+response.error);
					$('#credit-link-'+leadId).text(origText)
				}
				else
				{
					// Success
				}
			},
			false,
			false
		);

	}

};
