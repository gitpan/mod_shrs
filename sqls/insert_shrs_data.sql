
INSERT INTO USERS ( 
   user_account, password, last_name, first_name, 
	initials, email_address, is_active 
) VALUES ( 
   'Unspecified', 'password', 'Unspecified', '', '', '', false); 

INSERT INTO USERS ( 
   user_account, password, last_name, first_name, initials, email_address, is_active 
) VALUES ( 
   'Admin', 'rmhwswc', 'Admin', '', '', '', true);

INSERT INTO USERS ( 
   user_account, password, last_name, first_name, initials, email_address, is_active 
) VALUES ( 
   'guest', 'guest', 'guest', 'guest', 'g', 'mail@johnatkins.net', true);

INSERT INTO USERS ( 
   user_account, password, 
   last_name, first_name, initials, email_address, is_active 
) VALUES ( 
   'smith', 'password', 'Smith', 'Jim', 'js', 'js@johnatkins.net', true);
  
INSERT INTO USERS ( 
   user_account, password, last_name, first_name, initials, email_address, is_active 
) VALUES ( 
   'johnson', 'password', 'Johnson', 'James', 'js', 'jj@johnatkins.net', true);

INSERT INTO USERS ( 
   user_account, password, last_name, first_name, initials, email_address, is_active  
) VALUES ( 
   'roberts', 'password', 'Roberts', 'Bob', '', 'br@johnatkins.net', true);

INSERT INTO system (
   system_oid, qa_mgr_oid, comp_super_oid, market_mgr_oid
) VALUES ( 
   1, 2, 2, 2 );


INSERT INTO component_types ( comp_type ) VALUES ( 'Unspecified' ); 
INSERT INTO component_types ( comp_type ) VALUES ( 'Documentation' ); 
INSERT INTO component_types ( comp_type ) VALUES ( 'Hardware' );
INSERT INTO component_types ( comp_type ) VALUES ( 'Packaging/Peripherals' );
INSERT INTO component_types ( comp_type ) VALUES ( 'Tools and Process' );
INSERT INTO component_types ( comp_type ) VALUES ( 'Software' );


INSERT INTO components ( component, comp_type_oid, user_oid )  
	VALUES ( 'Unspecified', 6, 1 );
INSERT INTO components ( component, comp_type_oid, user_oid )  
	VALUES ( 'Request System', 6, 2 );
INSERT INTO components ( component, comp_type_oid, user_oid )  
	VALUES ( 'LDAP API', 6,  3);
INSERT INTO components ( component, comp_type_oid, user_oid ) 
	VALUES ( 'IMAP API', 6, 3);
INSERT INTO components ( component, comp_type_oid, user_oid ) 
	VALUES ( 'BIOS', 6, 3 );
INSERT INTO components ( component, comp_type_oid, user_oid )  
	VALUES ( 'Bootstrap', 6, 3 );
INSERT INTO components ( component, comp_type_oid, user_oid )  
	VALUES ( 'Flash Image', 6, 3 );
INSERT INTO components ( component, comp_type_oid, user_oid )  
	VALUES ( 'Hard Disk', 3, 4 );
INSERT INTO components ( component, comp_type_oid, user_oid )  
	VALUES ( 'Mother Board', 3, 4 );
INSERT INTO components ( component, comp_type_oid, user_oid )  
	VALUES ( 'Serial Port', 3, 4 );
INSERT INTO components ( component, comp_type_oid, user_oid )  
	VALUES ( 'Modem', 3, 4 );
INSERT INTO components ( component, comp_type_oid, user_oid )  
	VALUES ( 'Packaging', 4, 4 );


INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( 'Unspecified', 1 );

INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.0', 2 );

INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.0', 3 );
INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.1', 3 );

INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.0', 4 );
INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.1', 4 );
INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.2', 4 );

INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.0', 5 );
INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.1', 5 );

INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( 'B18', 6 );
INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( 'B19', 6 );

INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.0', 7 );
INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.1', 7 );

INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.0', 8 );
INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.1', 8 );

INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.0', 9 );
INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.1', 9 );

INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.0', 10 );
INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.1', 10 );

INSERT INTO component_revisions ( comp_rev, component_oid ) 
	VALUES ( '1.0', 11 );


INSERT INTO request_types ( request_type )
	VALUES ( 'Unspecified' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'New Requirement' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Software Requirements');
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Software Design' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Software Code' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Software Documentation' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Software Test Case' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Software Test Tool' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Software Compilers' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Software Other' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Hardware' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Operating System' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'User Error' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Operations Error' );
INSERT INTO request_types ( request_type ) 
	VALUES ( 'Don''t Know' );


INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Unspecified' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Synthesis of Design' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Synthesis of Code' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Synthesis of Test Procedure' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Synthesis of User Publication' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Inspection of Requirements' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Inspection of Detailed Design' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Inspection of Operational Documentation' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Inspection of Software Module' ); 
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Inspection of Test Procedure' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Formal Review of Plans' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Formal Review of Requirements' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Formal Review of Preliminary Design' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Formal Review of Critical Design' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Formal Review of Test Readiness' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Formal Review of Formal Qualifications' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Testing Planning' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Testing Module' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Testing Component' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Testing Configuration Item' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Testing Integrate And Test' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Testing Independent Verification And Validation' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Testing System' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Testing Test and Evaluate' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Testing Acceptance' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Customer Support Production Development' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Customer Support Installation' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'Customer Support Operation' );
INSERT INTO finding_activities ( finding_activity ) 
	VALUES ( 'New Requirement' ); 


INSERT INTO originators ( originator ) VALUES ( 'Unspecified' );
INSERT INTO originators ( originator ) VALUES ( 'JohnAtkins.Net' );
INSERT INTO originators ( originator ) VALUES ( 'Client A' );
INSERT INTO originators ( originator ) VALUES ( 'Client B' );


INSERT INTO platforms ( platform ) VALUES ( 'Unspecfied' );
INSERT INTO platforms ( platform ) VALUES ( 'Don''t Know' );
INSERT INTO platforms ( platform ) VALUES ( 'Windows' );
INSERT INTO platforms ( platform ) VALUES ( 'OpenBSD' );
INSERT INTO platforms ( platform ) VALUES ( 'FreeBSD' );
INSERT INTO platforms ( platform ) VALUES ( 'NetBSD' );
INSERT INTO platforms ( platform ) VALUES ( 'Debian' );
INSERT INTO platforms ( platform ) VALUES ( 'Mandrake' );
INSERT INTO platforms ( platform ) VALUES ( 'Red Hat' );


INSERT INTO products ( product ) VALUES ( 'Unspecified' );
INSERT INTO products ( product ) VALUES ( 'Request System' );

INSERT INTO pam_releases ( pam_release, product_oid ) 
	VALUES ( 'Unspecified', 1 );
INSERT INTO pam_releases ( pam_release, product_oid ) 
	VALUES ( '1.0', 2 );

INSERT iNTO marketing_revisions ( marketing_rev, product_oid ) 
	VALUES ( 'Unspecified', 1 );
INSERT iNTO marketing_revisions ( marketing_rev, product_oid ) 
	VALUES ( '1.0', 2 );

INSERT INTO resolutions ( resolution ) VALUES ( 'Unspecified' );
INSERT INTO resolutions ( resolution ) VALUES ( 'Can''t Reproduce' );
INSERT INTO resolutions ( resolution ) VALUES ( 'Deferred' );
INSERT INTO resolutions ( resolution ) VALUES ( 'Documentation' );
INSERT INTO resolutions ( resolution ) VALUES ( 'Duplicat' );
INSERT INTO resolutions ( resolution ) VALUES ( 'Implemented in SW' );
INSERT INTO resolutions ( resolution ) VALUES ( 'Implemented in HW' );
INSERT INTO resolutions ( resolution ) VALUES ( 'Superceded' );
INSERT INTO resolutions ( resolution ) VALUES ( 'Under Investigation' );
INSERT INTO resolutions ( resolution ) VALUES ( 'Working as Intended' );
