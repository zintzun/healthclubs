<?

	/*
	* How to set up a new action within the JV Framework using a controller
	*
	* 1) Create a new dir in actions...
	*    e.g actions/my-test
	*
	* 2) Create a controller file... 
	*    e.g actions/my-test/_my_test_controller.proc
	*    (Based on the action name. File MUST use underscores _ instead of -'s)
	*
	* 3) Create a controller class inside the controller file...
	*    (Based on the action name. Class MUST use underscores _ instead of -'s)
	*
	*    class my_test_controller extends controller
	*    {
	*      // Custom code goes here
	*    }
	*
	* 4) Create an index function inside the controller class...
	*
	*    class my_test_controller extends controller
	*    {
	*			public function index()
	*			{
	*				// Do some stuff
	*			}
	*    }
	*
	* 5) Inside the index function register 'hello world' to a template variable...
	*
	*    class controller extends base_controller
	*    {
	*			public function index()
	*			{
	*				// Do some stuff
	*				$this->v->my_example_var = "Hello world";
	*			}
	*    }
	*
	* 6) Create the index template/view file...
	*    e.g. actions/my-test/index.htm
	*
	* 7) In the template index.htm print out the controller variable we set...
	*    e.g. In the file actions/my-test/index.htm write this
	*
	*    <h1><?= $__my_example_var; ?></h1>
	*
	* 8) Run the action
	*    http://local-tds/my-test
	*
	* 9) Create a new functioncalled page1 & page 2 with some example output
	*
	*    class controller extends base_controller
	*    {
	*			public function page1()
	*			{
	*				// Do some stuff
	*				$this->v->my_title = "This is page 1";
	*			}
	*    }
	*
	*    class controller extends base_controller
	*    {
	*			public function page2()
	*			{
	*				// Do some stuff
	*				$this->v->my_title = "This is page 2";
	*			}
	*    }
	*
	* 10) Now run the new actions in the browser
	*
	*     http://local-tds/my-test/page1
	*     http://local-tds/my-test/page2
	*
	* 11) go here to get some more ideas on how to work with
	*     the template variables
	*
	*     http://local-tds/my-test/page3
	*        
	*/

	class my_test_controller extends controller
	{

		/*
		* This deals with the business logic for the index
		*/
		
		public function index()
		{
			// In this part you would do database look-ups 
			
			// After the lookups you can assign the template variables
			$this->v->my_example_var = "Hello world";
		}
		
		/*
		* This deals with the business logic for page1
		*/
		
		public function page1()
		{
			$this->v->my_title = "This is page 1";
		}

		/*
		* This deals with the business logic for page2
		*/

		public function page2()
		{
			$this->v->my_title = "This is page 2";
		}

		/*
		* This deals with the business logic for page3
		*/

		public function page3()
		{

			// Example sending title to the template (can be any variable name)
			$this->v->title = "This is page 3";
			
			// Example sending array values to the template
			$this->v->my_array = array
			(
				'First value in my array',
				'Second thing in my array',
			);
		}
	
	}

?>