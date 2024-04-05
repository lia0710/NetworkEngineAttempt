#pragma once

#ifndef  _ASSET_H_
#define _ASSET_H_

#include "Object.h"

class Asset : public Object
{
	DECLARE_ABSTRACT_DERIVED_CLASS(Asset, Object)

public:
	void Load(json::JSON&) override;
	void SetFilepath(std::string& path) { filepath = path; };

protected:
	std::string filepath;

	friend class AssetManager;
};

#endif // !_ASSET_H_
