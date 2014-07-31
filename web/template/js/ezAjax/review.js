/*
* Review
*/

ezAjax.review = 
{
	// This funny incrementor is used when drawing stars because the star
	// plugin doesnt work correcctly unless its a totaly new set of stars each time
	curStarSet : 0,
	
	editingReview :
	{
		id : false,
		review : false,
		rating : false,
		stars : false
	},

	/*
	* Validate form for new reviews only.
	*/
	validateForm : function()
	{
		$('#new-name').val(jQuery.trim($('#new-name').val()));
		$('#new-email').val(jQuery.trim($('#new-email').val()));
		$('#new-url').val(jQuery.trim($('#new-url').val()));
		$('#new-review').val(jQuery.trim($('#new-review').val()));
		
		if (! $('#new-name').val())
		{
			alert("Please write your name.");
			return false;
		}
		if ( !ezAjax.tools.checkEmail($('#new-email').val()) )
		{
			alert("Please provide a valid e-mail.");
			return false;
		}
		if ($('#new-url').val() && ! ezAjax.tools.checkURL($('#new-url').val()) )
		{
			alert("Please provide a valid URL address.");
			return false;
		}
		if (!$('#new-review').val())
		{
			alert("Please write a review for this business.");
			return false;
		}
		if ( parseInt($('#value-stars-new').val()) < 0  || parseInt($('#value-stars-new').val()) > 5 )
		{
			alert("Invalid rating!!."+$('#value-stars-new').val());
			return false;
		}
		else if ( ! parseInt($('#value-stars-new').val()) > 0 ) {
			alert("Please select a star to rate this business.");
			return false;
		}
		
		return true;
	},

	/*
	* Edit a previously posted review.
	* reviewId: is a usually a numeric value but can be a string.
	* rating: numeric value.
	*/
	edit : function(reviewId,rating)
	{
		// Close any other opened edit going on
		ezAjax.review.cancelEdit();

		if ( ezAjax.review.editingReview.id == reviewId )
		{
			rating = ezAjax.review.editingReview.rating;
		}

		// Store the review we are editing in case we need to cancel
		ezAjax.review.storeLastEdited(reviewId,rating);
	
		// Create the edit review form
		$('#review-'+reviewId).html(ezAjax.review.reviewForm(reviewId,rating,$('#review-'+reviewId+' .body span').html()));

		//Bind stars to mouse actions (mouseover, mouseout, click).
		//The container id is prefixed with "star"
		bindStarsActions('stars'+reviewId);

	},

	/*
	* Store the review we are editing in case we need to cancel
	*/
	
	storeLastEdited : function(reviewId,rating)
	{
		ezAjax.review.editingReview.id     = reviewId;
		ezAjax.review.editingReview.review = $('#review-'+reviewId).html();
		ezAjax.review.editingReview.rating = rating;
		ezAjax.review.editingReview.stars  = $('#review-'+reviewId+' span').html();
	},

	/*
	* If cancel button is pressed, the review gets the previous values.
	*/
	
	cancelEdit : function()
	{
		if ( ezAjax.review.editingReview.id !== false )
		{
			$('#review-'+ezAjax.review.editingReview.id).html(ezAjax.review.editingReview.review);
		}
	},


	/*
	* Form with rating stars, textarea, "cancel" and "Edit Review" buttons.
	*/
	reviewForm : function(reviewId,rating,text)
	{
		return '<div id="edit-review-panel">'+
		'<div id="stars'+reviewId+'">'+ezAjax.review.editingReview.stars+ '</div>'+
		'<br />'+
		'<textarea id="review-edit">'+text+'</textarea><br />'+
		'<input type="button" value="Cancel" onClick="ezAjax.review.cancelEdit()"> '+
		'<input type="button" value="Edit Review" onClick="ezAjax.review.postEdit()">'+
		'</div>';
	},

	/*
	* This functions creates the object ezAja.ajax which will be used to post the 
	* form to the server.
	* Validates the the form fields: review, reviewId & rating.
	*/
	
	postEdit : function()
	{
		// Get the review that the user just entered
		$('#review-edit').val(jQuery.trim($('#review-edit').val()));
		var review = $('#review-edit').val();
		
		// Get the ID of the review we are currently editing
		var reviewId = ezAjax.review.editingReview.id;

		// Get the rating that the user just entered
		var rating = $('#value-stars'+reviewId).val();
		
		if ( rating <= 0 ) 
		{
			alert("Please enter a star rating");
			return false;
		}
		if ( ! review )
		{
			alert("Please write a review for this business.");
			return false;
		}

		var stars = ezAjax.review.editingReview.stars  = $('#stars'+reviewId).html();

		//Display "loading icon" while the request is processing.
		$('#review-'+reviewId).html('<img src="/template/gfx/loading-small.gif" />');
		
		ezAjax.ajax
		( 
			'review_edit', 
			{
				reviewId: reviewId,
				rating: rating,
				review: review
			},

			function(response)
			{
				if (response.error != 'OK')
				{
					alert(response.error);
				}
				else
				{
					ezAjax.review.cancelEdit();
					$('#review-'+reviewId+' .body span').html(review);
					$('#value-stars'+reviewId).val(rating);
					$('#stars'+reviewId).html(ezAjax.review.editingReview.stars);
					
					ezAjax.review.editingReview.id = false;
				}
			},
			false,
			false
		);

	}

};
