/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef UNITDRAWER_STATE_H
#define UNITDRAWER_STATE_H

#include <array>
#include "System/type2.h"

struct float4;
class CUnitDrawer;
class CCamera;
struct ISkyLight;

namespace Shader {
	struct IProgramObject;
}

enum {
	DRAWER_STATE_FFP = 0, // fixed-function path
	DRAWER_STATE_SSP = 1, // standard-shader path (ARB/GLSL)
	DRAWER_STATE_SEL = 2, // selected path
	DRAWER_STATE_CNT = 3,
};


struct IUnitDrawerState {
public:
	static IUnitDrawerState* GetInstance(bool haveARB, bool haveGLSL);
	static void FreeInstance(IUnitDrawerState* state) { delete state; }

	static void PushTransform(const CCamera* cam);
	static void PopTransform();
	static float4 GetTeamColor(int team, float alpha);

	IUnitDrawerState() { modelShaders.fill(nullptr); }
	virtual ~IUnitDrawerState() {}

	virtual bool Init(const CUnitDrawer*) { return false; }
	virtual void Kill() {}

	virtual bool CanEnable(const CUnitDrawer*) const { return false; }
	virtual bool CanDrawAlpha() const { return false; }
	virtual bool CanDrawDeferred() const { return false; }

	virtual void Enable(const CUnitDrawer*, bool, bool) = 0;
	virtual void Disable(const CUnitDrawer*, bool) = 0;

	virtual void EnableTextures(const CUnitDrawer*) const = 0;
	virtual void DisableTextures(const CUnitDrawer*) const = 0;
	virtual void EnableShaders(const CUnitDrawer*) {}
	virtual void DisableShaders(const CUnitDrawer*) {}

	virtual void UpdateCurrentShaderSky(const CUnitDrawer*, const ISkyLight*) const {}
	virtual void SetTeamColor(int team, const float2 alpha) const = 0;
	virtual void SetNanoColor(const float4& color) const {}

	void SetActiveShader(unsigned int shadowed, unsigned int deferred) {
		// shadowed=1 --> shader 1 (deferred=0) or 3 (deferred=1)
		// shadowed=0 --> shader 0 (deferred=0) or 2 (deferred=1)
		modelShaders[MODEL_SHADER_ACTIVE] = modelShaders[shadowed + deferred * 2];
	}

	enum ModelShaderProgram {
		MODEL_SHADER_NOSHADOW_STANDARD = 0, ///< model shader (V+F) without self-shadowing
		MODEL_SHADER_SHADOWED_STANDARD = 1, ///< model shader (V+F) with self-shadowing
		MODEL_SHADER_NOSHADOW_DEFERRED = 2, ///< deferred version of MODEL_SHADER_NOSHADOW (GLSL-only)
		MODEL_SHADER_SHADOWED_DEFERRED = 3, ///< deferred version of MODEL_SHADER_SHADOW (GLSL-only)

		MODEL_SHADER_ACTIVE            = 4, ///< currently active model shader
		MODEL_SHADER_COUNT             = 5,
	};

protected:
	// shared ARB and GLSL state managers
	void EnableCommon(const CUnitDrawer*, bool);
	void DisableCommon(const CUnitDrawer*, bool);
	void EnableTexturesCommon(const CUnitDrawer*) const;
	void DisableTexturesCommon(const CUnitDrawer*) const;

protected:
	std::array<Shader::IProgramObject*, MODEL_SHADER_COUNT> modelShaders;
};




struct UnitDrawerStateFFP: public IUnitDrawerState {
public:
	bool CanEnable(const CUnitDrawer*) const;

	void Enable(const CUnitDrawer*, bool, bool);
	void Disable(const CUnitDrawer*, bool);

	void EnableTextures(const CUnitDrawer*) const;
	void DisableTextures(const CUnitDrawer*) const;

	void SetTeamColor(int team, const float2 alpha) const;
};


struct UnitDrawerStateARB: public IUnitDrawerState {
public:
	bool Init(const CUnitDrawer*);
	void Kill();

	bool CanEnable(const CUnitDrawer*) const;

	void Enable(const CUnitDrawer*, bool, bool);
	void Disable(const CUnitDrawer*, bool);

	void EnableTextures(const CUnitDrawer*) const;
	void DisableTextures(const CUnitDrawer*) const;
	void EnableShaders(const CUnitDrawer*);
	void DisableShaders(const CUnitDrawer*);

	void SetTeamColor(int team, const float2 alpha) const;
};


struct UnitDrawerStateGLSL: public IUnitDrawerState {
public:
	bool Init(const CUnitDrawer*);
	void Kill();

	bool CanEnable(const CUnitDrawer*) const;
	bool CanDrawAlpha() const { return true; }
	bool CanDrawDeferred() const { return true; }

	void Enable(const CUnitDrawer*, bool, bool);
	void Disable(const CUnitDrawer*, bool);

	void EnableTextures(const CUnitDrawer*) const;
	void DisableTextures(const CUnitDrawer*) const;
	void EnableShaders(const CUnitDrawer*);
	void DisableShaders(const CUnitDrawer*);

	void UpdateCurrentShaderSky(const CUnitDrawer*, const ISkyLight*) const;
	void SetTeamColor(int team, const float2 alpha) const;
	void SetNanoColor(const float4& color) const;
};

#endif

