<?php

	/*
	* Dealing with the site unit tests
	*/

	class tests_controller
	{

		/*
		* Default action for /admin
		*/

		public function tests_index()
		{
			$this->_validate();
			$this->v->tests = $this->get_test_file_paths();
		}

		/*
		* Default action for /admin
		*/

		public function tests_run()
		{
			$test = &new FileGroupTest();
			$test->run(new HtmlReporter());
			exit;
		}
		
		
		/*
		* Make sure it's an active admin session
		*/
		
		private function _validate()
		{
			if ( ! session_is_admin() && ! IS_ADMIN_IP )
			{
				notify_redirect('/');
			}
		}
		
		/*
		* Returns all the paths to the test files as two arrays: existing_tests and missing_tests
		*/

		public function get_test_file_paths()
		{
			$existing_tests = array();
			$missing_tests = array();
			
			foreach (array('lib','interfaces','interfaces/biz','actions') as $dir )
			{
				foreach (dir_to_array($dir) as $file) 
				{
					if ( strpos($file, '.inc') )
					{
						$file_path = 'unit-tests/'.str_replace('.inc', '.tests', $file);
					}
					else if ( $dir == 'actions' )
					{
						$file_path = "unit-tests/$file.tests";
					}
						
					if ( file_exists($file_path) )
					{
						$existing_tests[] = $file_path;
					}
					else
					{
						$missing_tests[] = $file_path;
					}

			
				}
			}
		
			return (object) array
			(	
				'existing_tests' => $existing_tests,
				'missing_tests' => $missing_tests
			);	
			
		}
	
	}

	/**
	 * Simple test includes
	 */

	include 'third_party/simpletest/unit_tester.php';
	include 'third_party/simpletest/web_tester.php';
	include 'third_party/simpletest/reporter.php';
	include 'third_party/simpletest/xml.php';

	class FileGroupTest extends GroupTest
	{
		function FileGroupTest()
		{
			global $TARGET;
			
			$this->GroupTest('Run all tests!');

			if ( isset($TARGET->arg3) && $TARGET->arg3 )
			{

				$test_path = "unit-tests/{$TARGET->arg3}.tests";

				if ( !file_exists($test_path) )
				{
					die("Could not find: {$test_path}");	
				}

				$this->addTestFile($test_path);
			}
			else
			{
				$tests_controller = new tests_controller;
				
				$ar = $tests_controller->get_test_file_paths();
	
				foreach ($ar->existing_tests as $test_file)
				{
					$this->addTestFile($test_file);
				}
			}
		}
	}

?>
