#ifndef TEST_DICTIONARY_H
#define TEST_DICTIONARY_H

#include <unordered_map>
#include <string>
#include <array>

#define MAX_ESTRING_LENGTH 128

class Dictionary {
public:

	bool												IsEmpty() const { return args.empty(); }
	void												Clear();
	void												Remove( const char *key );
	void												Set( const char *key, const char *value );
	void												SetFloat( const char *key, float val );
	void												SetInt( const char *key, int val );
	void												SetBool( const char *key, bool val );

															// these return default values of 0.0, 0 and false
	const char *										GetString( const char *key, const char *defaultString = "" ) const;
	float												GetFloat( const char *key, const char *defaultString ) const;
	int													GetInt( const char *key, const char *defaultString ) const;
	bool												GetBool( const char *key, const char *defaultString ) const;

private:

	std::unordered_map<std::string, std::string>		args;
	std::array<char, MAX_ESTRING_LENGTH>				setBuffer;

};

inline void Dictionary::Clear() {
	args.clear();
	setBuffer.fill(0);
}

inline void Dictionary::Remove( const char *key ) {
	if ( key == NULL || key[0] == '\0' )
		return;

	const auto & iter = args.find(key);
	if ( iter != args.end() )
		args.erase(key);
}

inline void Dictionary::Set( const char *key, const char *value ) {
	if ( key == NULL || key[0] == '\0' )
		return;

	args[key] = value;
}

inline void Dictionary::SetFloat( const char *key, float val ) {
	snprintf(setBuffer.data(), setBuffer.size(), "%f", val);
	Set( key, setBuffer.data() );
}

inline void Dictionary::SetInt( const char *key, int val ) {
	snprintf(setBuffer.data(), setBuffer.size(), "%i", val);
	Set( key, setBuffer.data() );
}

inline void Dictionary::SetBool( const char *key, bool val ) {
	snprintf(setBuffer.data(), setBuffer.size(), "%i", val);
	Set( key, setBuffer.data() );
}

inline const char * Dictionary::GetString( const char *key, const char *defaultString ) const {
	const auto & iter = args.find(key);
	if ( iter != args.end() )
		return iter->second.c_str();

	return defaultString;
}

inline float Dictionary::GetFloat( const char *key, const char *defaultString ) const {
	return (float)atof( GetString( key, defaultString ) );
}

inline int Dictionary::GetInt( const char *key, const char *defaultString ) const {
	return atoi( GetString( key, defaultString ) );
}

inline bool Dictionary::GetBool( const char *key, const char *defaultString ) const {
	return ( atoi( GetString( key, defaultString ) ) != 0 );
}

#endif /* TEST_DICTIONARY_H */