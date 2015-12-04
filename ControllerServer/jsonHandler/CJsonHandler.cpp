/*
 * CJsonHandler.cpp
 *
 *  Created on: 2015年12月4日
 *      Author: Louis Ju
 */

#include "CJsonHandler.h"
#include <json.h>
#include <stdio.h>
#include <sstream>

using namespace std;

static CJsonHandler *mInstance = 0;

template<class T>
string ConvertToString(T value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

CJsonHandler::CJsonHandler()
{

}

CJsonHandler::~CJsonHandler()
{

}

CJsonHandler* CJsonHandler::getInstance()
{
	if ( !mInstance )
	{
		mInstance = new CJsonHandler();
	}
	return mInstance;
}

void json_parse_array(json_object *jobj, char *key, map<string, string> &mapData)
{
	enum json_type type;
	json_object * jsonObject;

	int json_parse(json_object * jobj, map<string, string> &mapData); /*Forward Declaration*/
	json_object *jarray = jobj; /*Simply get the array*/
	if ( key )
	{
		json_object_object_get_ex( jobj, key, &jarray ); /*Getting the array if it is a key value pair*/
	}

	int arraylen = json_object_array_length( jarray ); /*Getting the length of the array*/
	printf( "Array Length: %dn", arraylen );
	int i;
	json_object * jvalue;

	for ( i = 0; i < arraylen ; i++ )
	{
		jvalue = json_object_array_get_idx( jarray, i ); /*Getting the array element at position i*/
		type = json_object_get_type( jvalue );
		if ( type == json_type_array )
		{
			json_parse_array( jvalue, NULL, mapData );
		}
		else if ( type != json_type_object )
		{
			type = json_object_get_type( jvalue );
			switch ( type )
			{
				case json_type_boolean:
					mapData[key] = json_object_get_boolean( jobj ) ? "true" : "false";
					break;
				case json_type_double:
					mapData[key] = ConvertToString( json_object_get_double( jobj ) );
					break;
				case json_type_int:
					mapData[key] = ConvertToString( json_object_get_int( jobj ) );
					break;
				case json_type_string:
					mapData[key] = json_object_get_string( jobj );
					break;
				case json_type_object:
					if ( json_object_object_get_ex( jobj, key, &jsonObject ) )
					{
						json_parse( jsonObject, mapData );
					}
					break;
				case json_type_array:
					json_parse_array( jvalue, key, mapData );
					break;
				case json_type_null:
					break;
			}
		}
		else
		{
			json_parse( jvalue, mapData );
		}
	}
}

int json_parse(json_object * jobj, map<string, string> &mapData)
{
	json_object * jsonObject;
	enum json_type type;
	json_object_object_foreach( jobj, key, val )
	{
		type = json_object_get_type( val );
		switch ( type )
		{
			case json_type_boolean:
				mapData[key] = json_object_get_boolean( jobj ) ? "true" : "false";
				break;
			case json_type_double:
				mapData[key] = ConvertToString( json_object_get_double( jobj ) );
				break;
			case json_type_int:
				mapData[key] = ConvertToString( json_object_get_int( jobj ) );
				break;
			case json_type_string:
				mapData[key] = json_object_get_string( jobj );
				break;
			case json_type_object:
				if ( json_object_object_get_ex( jobj, key, &jsonObject ) )
				{
					json_parse( jsonObject, mapData );
				}
				break;
			case json_type_array:
				json_parse_array( jobj, key, mapData );
				break;
			case json_type_null:
				break;
		}
	}
	return mapData.size();
}

int CJsonHandler::parse(string strJSON, map<string, string> &mapData)
{
	json_object * jobj = json_tokener_parse( strJSON.c_str() );

	json_parse( jobj, mapData );

	/*	enum json_type type;
	 json_object_object_foreach( jobj, key, val )
	 {
	 type = json_object_get_type( val );
	 switch ( type )
	 {
	 case json_type_boolean:
	 mapData[key] = json_object_get_boolean( jobj ) ? "true" : "false";
	 break;
	 case json_type_double:
	 mapData[key] = ConvertToString( json_object_get_double( jobj ) );
	 break;
	 case json_type_int:
	 mapData[key] = ConvertToString( json_object_get_int( jobj ) );
	 break;
	 case json_type_string:
	 mapData[key] = json_object_get_string( jobj );
	 break;
	 case json_type_object:
	 printf( "json_type_objectn" );
	 break;
	 case json_type_array:
	 printf( "type: json_type_array, " );
	 //json_parse_array( jobj, key );
	 break;
	 case json_type_null:
	 break;
	 }
	 }
	 */
	return mapData.size();
}
