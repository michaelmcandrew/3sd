<?php

/*
 +--------------------------------------------------------------------+
 | CiviCRM version 3.3                                                |
 +--------------------------------------------------------------------+
 | Copyright CiviCRM LLC (c) 2004-2010                                |
 +--------------------------------------------------------------------+
 | This file is a part of CiviCRM.                                    |
 |                                                                    |
 | CiviCRM is free software; you can copy, modify, and distribute it  |
 | under the terms of the GNU Affero General Public License           |
 | Version 3, 19 November 2007 and the CiviCRM Licensing Exception.   |
 |                                                                    |
 | CiviCRM is distributed in the hope that it will be useful, but     |
 | WITHOUT ANY WARRANTY; without even the implied warranty of         |
 | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               |
 | See the GNU Affero General Public License for more details.        |
 |                                                                    |
 | You should have received a copy of the GNU Affero General Public   |
 | License and the CiviCRM Licensing Exception along                  |
 | with this program; if not, contact CiviCRM LLC                     |
 | at info[AT]civicrm[DOT]org. If you have questions about the        |
 | GNU Affero General Public License or the licensing of CiviCRM,     |
 | see the CiviCRM license FAQ at http://civicrm.org/licensing        |
 +--------------------------------------------------------------------+
*/

/**
 *
 * @package CRM
 * @copyright CiviCRM LLC (c) 2004-2010
 * $Id$
 *
 */

require_once 'CRM/Core/DAO/Tag.php';

class CRM_Core_BAO_Tag extends CRM_Core_DAO_Tag
{
    /**
     * class constructor
     */
    function __construct( )
    {
        parent::__construct( );
    }

    /**
     * Takes a bunch of params that are needed to match certain criteria and
     * retrieves the relevant objects. Typically the valid params are only
     * contact_id. We'll tweak this function to be more full featured over a period
     * of time. This is the inverse function of create. It also stores all the retrieved
     * values in the default array
     * 
     * @param array $params      (reference ) an assoc array of name/value pairs
     * @param array $defaults    (reference ) an assoc array to hold the flattened values
     * 
     * @return object     CRM_Core_DAO_Tag object on success, otherwise null
     * @access public
     * @static
     */
    static function retrieve( &$params, &$defaults )
    {
        $tag = new CRM_Core_DAO_Tag( );
        $tag->copyValues( $params );
        if ( $tag->find( true ) ) {
            CRM_Core_DAO::storeValues( $tag, $defaults );
            return $tag;
        }
        return null;
    }

    function getTree ( $usedFor = null, $excludeHidden = false )
    {
        if ( !isset ( $this->tree ) ) {
            $this->buildTree( $usedFor, $excludeHidden );
        }
        return $this->tree;
    }
	
    function buildTree( $usedFor = null, $excludeHidden = false )
    {
        $sql = "SELECT civicrm_tag.id, civicrm_tag.parent_id,civicrm_tag.name FROM civicrm_tag ";
        
        $whereClause = array( );
        if ( $usedFor ) {
            $whereClause[] = "used_for like '%{$usedFor}%'";
        }
        if ( $excludeHidden ) {
            $whereClause[] = "is_tagset = 0";
        }
        
        if ( !empty( $whereClause ) ) {
            $sql .= " WHERE ". implode( ' AND ', $whereClause );  
        }

        $sql .= " ORDER BY parent_id,name";

        $dao =& CRM_Core_DAO::executeQuery( $sql, CRM_Core_DAO::$_nullArray, true, null, false, false );

        $orphan = array();
        while ( $dao->fetch( ) ) {
            if ( !$dao->parent_id ) {
                $this->tree[$dao->id]['name'] = $dao->name;
            } else {
                if ( array_key_exists( $dao->parent_id, $this->tree ) ) {
                    $parent =& $this->tree[$dao->parent_id];
                    if ( !isset ($this->tree[$dao->parent_id]['children'] ) ) {
                        $this->tree[$dao->parent_id]['children'] = array();
                    }
                }
                else {
                    //3rd level tag
                    if ( !array_key_exists($dao->parent_id, $orphan ) ) {
                        $orphan[$dao->parent_id]=array('children'=> array());
                    }
                    $parent=& $orphan[$dao->parent_id];
                }
                $parent['children'][$dao->id] = array ('name'=>$dao->name);
            }
        }
        if ( sizeof( $orphan ) ) {
            //hang the 3rd level lists at the right place
            foreach ( $this->tree as &$level1 ) {
                if ( ! isset ( $level1['children'] ) ) {
                    continue;
                }

                foreach ( $level1['children'] as $key => &$level2 ) {
                    if ( array_key_exists( $key,$orphan ) ) {
                        $level2['children'] = $orphan[$key]['children'];
                    }
                }
            }
        }
    }

    function getTagsUsedFor( $usedFor = array( 'civicrm_contact' ),
                             $buildSelect = true,
                             $all = false,
                             $parentId = NULL )
    {       
        $tags = array( );

        if ( empty($usedFor) ) {
            return $tags;
        } 
        if ( !is_array($usedFor) ) {
            $usedFor = array( $usedFor );  
        }

        if ( $parentId === NULL ) {
            $parentClause = " parent_id IS NULL AND ";
        } else {
            $parentClause = " parent_id = {$parentId} AND ";
        }

        foreach( $usedFor as $entityTable ) { 
            $tag = new CRM_Core_DAO_Tag( );
            $tag->fields( );
            $tag->orderBy( 'parent_id' );
            if ( $buildSelect ) {
                $tag->whereAdd( "is_tagset = 0 AND {$parentClause} used_for LIKE '%{$entityTable}%'");
            } else {
                $tag->whereAdd( "used_for LIKE '%{$entityTable}%'");
            }
            if ( !$all ) {
                $tag->is_tagset = 0; 
            }
            $tag->find( );
            
            while( $tag->fetch( ) ) {
                if ( $buildSelect ) {
                    $tags[$tag->id] = $tag->name;
                } else {
                    $tags[$tag->id]['name']      = $tag->name;
                    $tags[$tag->id]['parent_id'] = $tag->parent_id;
                    $tags[$tag->id]['is_tagset'] = $tag->is_tagset;
                    $tags[$tag->id]['used_for']  = $tag->used_for;
                }
            }
            $tag->free( );
        }

        return $tags;
    }

    static function getTags( $usedFor = 'civicrm_contact', 
                             &$tags = array( ), 
                             $parentId = null, 
                             $separator = '&nbsp;&nbsp;', 
                             $flatlist = true )
    {
        $parentClause = '';
        if ( $parentId ) {
            $separator .= '&nbsp;&nbsp;';
            $parentClause = " parent_id = {$parentId}";
        } else {
            $separator = '';
            $parentClause = ' is_tagset = 0 AND parent_id IS NULL';
        }
        
        $query = "SELECT id, name, parent_id 
                  FROM civicrm_tag 
                  WHERE {$parentClause} AND used_for LIKE '%{$usedFor}%' ORDER BY name";
        
        $dao = CRM_Core_DAO::executeQuery( $query, CRM_Core_DAO::$_nullArray, true, null, false, false );
        
        while( $dao->fetch( ) ) {
            $tags[$dao->id] = $separator . $dao->name;
            self::getTags( $usedFor, $tags, $dao->id, $separator );
        }
        
        return $tags;        
    }
    
    /**
     * Function to delete the tag 
     *
     * @param int $id   tag id
     *
     * @return boolean
     * @access public
     * @static
     *
     */
    static function del ( $id )
    {
        // delete all crm_entity_tag records with the selected tag id
        require_once 'CRM/Core/DAO/EntityTag.php';
        $entityTag = new CRM_Core_DAO_EntityTag( );
        $entityTag->tag_id = $id;
        if ( $entityTag->find( ) ) {
            while ( $entityTag->fetch() ) {
                $entityTag->delete();
            }
        }
        
        // delete from tag table
        $tag = new CRM_Core_DAO_Tag( );
        $tag->id = $id;

        require_once 'CRM/Utils/Hook.php';
        CRM_Utils_Hook::pre( 'delete', 'Tag', $id, $tag );

        if ( $tag->delete( ) ) {
            CRM_Utils_Hook::post( 'delete', 'Tag', $id, $tag );
            CRM_Core_Session::setStatus( ts('Selected Tag has been Deleted Successfuly.') );
            return true;
        }
        return false;
    }

    /**
     * takes an associative array and creates a contact object
     * 
     * The function extract all the params it needs to initialize the create a
     * contact object. the params array could contain additional unused name/value
     * pairs
     * 
     * @param array  $params         (reference) an assoc array of name/value pairs
     * @param array  $ids            (reference) the array that holds all the db ids
     * 
     * @return object    CRM_Core_DAO_Tag object on success, otherwise null
     * @access public
     * @static
     */
    static function add( &$params, &$ids )
    {
        if ( ! self::dataExists( $params ) ) {
            return null;
        }

        $tag = new CRM_Core_DAO_Tag( );
        
        // if parent id is set then inherit used for and is hidden properties
        if ( CRM_Utils_Array::value( 'parent_id', $params ) ) {
            // get parent details
            $params['used_for' ] = CRM_Core_DAO::getFieldValue( 'CRM_Core_DAO_Tag', $params['parent_id'] , 'used_for' );
        }

        $tag->copyValues( $params );
        $tag->id = CRM_Utils_Array::value( 'tag', $ids );

        require_once 'CRM/Utils/Hook.php';
        $edit = ($tag->id) ? true : false;
        if ($edit) {
            CRM_Utils_Hook::pre( 'edit', 'Tag', $tag->id, $tag );
        } else {
            CRM_Utils_Hook::pre( 'create', 'Tag', null, $tag );
        }

        $tag->save( );
        
        if ($edit) {
            CRM_Utils_Hook::post( 'edit', 'Tag', $tag->id, $tag );
        } else {
            CRM_Utils_Hook::post( 'create', 'Tag', null, $tag );
        }
        
        // if we modify parent tag, then we need to update all children
        if ( $tag->parent_id === 'null' ) {
            CRM_Core_DAO::executeQuery( "UPDATE civicrm_tag SET used_for=%1 WHERE parent_id = %2", 
                                        array( 1 => array( $params['used_for'], 'String' ),
                                               2 => array( $tag->id , 'Integer' ) ) );
        }
        
        return $tag;
    }

    /**
     * Check if there is data to create the object
     *
     * @param array  $params         (reference ) an assoc array of name/value pairs
     *
     * @return boolean
     * @access public
     * @static
     */
    static function dataExists( &$params )
    {
        if ( !empty( $params['name'] ) ) {
            return true;
        }
        
        return false;
    }
    
    /**
     * Function to get the tag sets for a entity object
     *
     * @param string $entityTable entity_table
     *
     * @return array $tagSets array of tag sets
     * @access public
     * @static
     */
    static function getTagSet( $entityTable )
    {
        $tagSets = array( );
        $query = "SELECT name FROM civicrm_tag WHERE is_tagset=1 AND parent_id IS NULL and used_for LIKE '%{$entityTable}%'";
        $dao = CRM_Core_DAO::executeQuery( $query, CRM_Core_DAO::$_nullArray, true, null, false, false );
        while( $dao->fetch( ) ) {
           $tagSets[] = $dao->name;
        }
        return $tagSets;
    }
    
    /**
     * Function to get the tags that are not children of a tagset.
     *
     * @return $tags associated array of tag name and id
     * @access public
     * @static
     */
    static function getTagsNotInTagset( )
    {
        $tags = $tagSets = array( );
        // first get all the tag sets
        $query = "SELECT id FROM civicrm_tag WHERE is_tagset=1 AND parent_id IS NULL";
        $dao = CRM_Core_DAO::executeQuery( $query, CRM_Core_DAO::$_nullArray );
        while( $dao->fetch( ) ) {
           $tagSets[] = $dao->id;
        }
        
        $parentClause = '';
        if ( !empty( $tagSets ) ) {
            $parentClause = ' WHERE ( parent_id IS NULL ) OR ( parent_id NOT IN ( ' .implode( ',', $tagSets ) .' ) )';
        }
        
        // get that tags that don't have tagset as parent
        $query = "SELECT id, name FROM civicrm_tag {$parentClause}";
        $dao = CRM_Core_DAO::executeQuery( $query );
        while( $dao->fetch( ) ) {
           $tags[$dao->id] = $dao->name;
        }
                
        return $tags;
    }
    
}