	edit : function(reviewId)
	{
		$('#review-'+reviewId).html(ezAjax.review.reviewForm());
	},
	
	reviewForm : function()
	{
		return ezAjax.review.createStars()+
		'';
	},
	
	createStars : function(reviewId, rating)
	{
		for (var i=1;i<=5;i++)
		{
			html += '<input name="star'+reviewId+'" type="radio" value="'+i+'" class="star" />";
		}
		
		return html;
	}



	cache :
	{
		reviewId	: false,
		review 		: false,
		rating 		: false,
		selectorR	: false,
		selectorS	: false
	},

	edit : function(reviewId)
	{
		var t;
		if (ezAjax.review.cache.reviewId)
		{
			$('#submit_button').text('Write a Review');
			alert(" reset " + ezAjax.review.cache.reviewId);

			ezAjax.review.resetSpanAreas();
			ezAjax.review.cache.reviewId = false;
			ezAjax.review.cache.review = false;
			ezAjax.review.cache.rating = false;
			ezAjax.review.cache.rate = false;
		}

		//$('#submit_button').hide();
		selector = '#r' + reviewId + ' span';
		ezAjax.review.cache.reviewId = reviewId;
		ezAjax.review.cache.review = $(selector).text();
		ezAjax.review.cache.rating = $('#rating'+reviewId).text();
		ezAjax.review.cache.selectorR = selector;
		ezAjax.review.cache.selectorS = '#s' + reviewId + ' span';
		ezAjax.review.insertTextArea(selector, ezAjax.review.cache.review);
		ezAjax.review.createStars('#s' + reviewId + ' span', reviewId,ezAjax.review.cache.rating, 0);

	},

	insertTextArea : function(selector, text)
	{
		$(selector).html('<textarea id="review-textarea" rows="8" cols="60">' + text + '</textarea>');
	},

	resetSpanAreas : function()
	{
		$(ezAjax.review.cache.selectorR).text(ezAjax.review.cache.review);
		alert("resetSpanAreas selectorS" + ezAjax.review.cache.selectorS);
		//ezAjax.review.createStars(ezAjax.review.cache.selectorS,ezAjax.review.cache.reviewId ,ezAjax.review.cache.rating, 1);
		$(ezAjax.review.cache.selectorS).html(ezAjax.review.createFixedStars(ezAjax.review.cache.reviewId, ezAjax.review.cache.rating));
	},

	addReview : function(span_container)
	{
		alert(' lengtht = ' + $('#rnew_review_panel textarea').length);
		if ( $('#rnew_review_panel textarea').length > 0 )
		{
			$("#review_form").submit();
		}

		else
		{
			$('#submit_button').text('Submit »');
			//alert($('#submit_button').text());
			ezAjax.review.edit(span_container);
			
		}
	},

	createFixedStars : function(reviewId, rating)
	{

		var reviewId = reviewId*1024;
		var html = '';
		for (var i=1;i<=5;i++)
		{
			html += '<input name="star'+reviewId+'" type="radio" value="'+i+'" class="star star'+reviewId+'" disabled="disabled" '+ (i==rating?'checked="checked"':'') +"/>";
		}
		return html;
	},

	createStars : function(selector, reviewId, rating, disabled)
	{

		var reviewId = reviewId*1024;
		var html = '';
		alert("disabled " + disabled);

		for (var i=1;i<=5;i++)
		{
			html += '<input name="star'+reviewId+'" type="radio" value="'+i+'" class="star star'+reviewId+'" '+(disabled==1?'disabled="disabled" ':'') +(i==rating?'checked="checked"':'') +"/>";
		}
		alert(html);
		//alert(selector);
		$(selector).html(html);
		$(
			function()
			{ 
				$('.star'+reviewId).rating
				(
					{ 
						callback: function(value, link)
						{
							alert(value);
							$('#rating').val(value);
						}
					}
				); 
			}
		);
	}
