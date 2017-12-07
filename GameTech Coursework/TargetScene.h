#pragma once

#include <nclgl\NCLDebug.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\CommonUtils.h>

//Fully striped back scene to use as a template for new scenes.
class TargetScene : public Scene
{
public:
	TargetScene(const std::string& friendly_name)
		: Scene(friendly_name)
	{
	}

	virtual ~TargetScene()
	{
	}

	virtual void OnInitializeScene() override;

	void draw();

	void spawn();

	void checkBasketballs();

	void drawBasket();
	
	void drawScore(float time);

	int total_score;
	virtual void OnUpdateScene(float dt) override;
	
	private:
		std::vector<std::vector<GameObject*>> objects;
		std::vector<PhysicsNode*> basketballs;
		std::vector<float> scores;
		AABB basket;
		GLuint ballTex , courtTex, boardTex, ironTex;
		float acum_time;

};