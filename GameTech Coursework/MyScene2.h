#pragma once

#include <nclgl\NCLDebug.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\CommonUtils.h>

//Fully striped back scene to use as a template for new scenes.
class MyScene2 : public Scene
{
public:
	MyScene2(const std::string& friendly_name)
		: Scene(friendly_name)
	{
	}

	virtual ~MyScene2()
	{
	}


	float m_RampAngle;
	const Vector3 ss_pos = Vector3(-5.5f, 1.5f, -5.0f);
	const Vector3 pool_pos = Vector3(0, 1.5f, 0);

	virtual void OnInitializeScene() override
	{
		PhysicsEngine::Instance()->SetPaused(true);

		for (int i = 0; i < objects.size(); i++) {
			objects[i].clear();
		}
		objects.clear();
		
		const int dims = 18;
		const int dimsx = dims  * 0.5f;
		const int dimsz = dims  * 0.7f;
		float parts = 360.0f / (float)dims / 2.0f;
		auto create_ball_cloth = [&](const Vector3& offset, const Vector3& scale, float ballsize)
		{
			
			float m;
			for (int x = 0; x < dimsx; ++x)
			{
				float rad = DegToRad( x * parts );
				objects.push_back(std::vector<GameObject*>());
				for (int z = 0; z < dimsz; ++z)
				{
					if (z == 0)  m = 0.0f;
					else m = 1.0f;
					Vector4 col = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
					Vector3 pos = offset + Vector3(scale.x * x * 0.5, 0.0f, scale.z * z * 0.5);
				//	Vector3 pos = offset + Vector3(cos(rad*4.5f) * 2, - scale.y * z * 0.5, sin(rad*4.5f) * 2);
					GameObject* sphere = CommonUtils::BuildSphereObject(
						"",					// Optional: Name
						pos,				// Position
						ballsize,			// Half-Dimensions
						true,				// Physics Enabled?
						m,				// Physical Mass (must have physics enabled)
						true,				// Physically Collidable (has collision shape)
						false,
						col);// Render color
					this->AddGameObject(sphere);
					objects[x].push_back(sphere);

				}
			}
		};
		create_ball_cloth(Vector3(-3.0f, 10.5f, -2.5f), Vector3(0.5f, 0.5f, 0.5f), 0.06f);
		
		for (int i = 0; i < dimsx; i++) {
			for (int j = 0; j < dimsz; j++) {
					int indexi = i - 1;
					for (int c = 0; c < 3; c++) {
						int indexj = j - 1 + c;
						if (indexi < dimsx && indexi > -1 && indexj < dimsz && indexj > -1) {
							DistanceConstraint* constraint = new DistanceConstraint(
								objects[i][j]->Physics(),					//Physics Object A
								objects[indexi][indexj]->Physics(),					//Physics Object B
								objects[i][j]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
								objects[indexi][indexj]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
							PhysicsEngine::Instance()->AddConstraint(constraint);
						}

					}
				    indexi = i;
					for (int c = 0; c < 3; c+=2) {
						int indexj = j - 1 + c;
						if (indexi < dimsx && indexi > -1 && indexj < dimsz && indexj > -1) {
							DistanceConstraint* constraint = new DistanceConstraint(
								objects[i][j]->Physics(),					//Physics Object A
								objects[indexi][indexj]->Physics(),					//Physics Object B
								objects[i][j]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
								objects[indexi][indexj]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
							PhysicsEngine::Instance()->AddConstraint(constraint);
						}
					}
					indexi = i + 1;
					for (int c = 0; c < 3; c++) {
						int indexj = j - 1 + c;
						if (indexi < dimsx && indexi > -1 && indexj < dimsz && indexj > -1) {
							DistanceConstraint* constraint = new DistanceConstraint(
								objects[i][j]->Physics(),					//Physics Object A
								objects[indexi][indexj]->Physics(),					//Physics Object B
								objects[i][j]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
								objects[indexi][indexj]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
							PhysicsEngine::Instance()->AddConstraint(constraint);
						}
					}
				}
		}

		GameObject* ground = CommonUtils::BuildCuboidObject(
			"Ground",
			Vector3(0.0f, 0.0f, 0.0f),
			Vector3(20.0f, 1.0f, 20.0f),
			false,
			0.0f,
			false,
			false,
			Vector4(0.2f, 0.5f, 1.0f, 1.0f));

		this->AddGameObject(ground);

		

	}

	void draw() {

		for (int i = 0; i < objects.size(); i++) {
			for (int j = 0; j < objects[i].size()-1; j++) {
				NCLDebug::DrawThickLine(objects[i][j]->Physics()->GetPosition(), objects[i][j+1]->Physics()->GetPosition(), 0.4f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));	
			}
		}

		for (int i = 0; i < objects.size()-1; i++) {
			for (int j = 0; j < objects[i].size(); j++) {
				NCLDebug::DrawThickLine(objects[i][j]->Physics()->GetPosition(), objects[i + 1][j]->Physics()->GetPosition(), 0.4f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));

			}
		}

	}

	float m_AccumTime;
	virtual void OnUpdateScene(float dt) override
	{
		Scene::OnUpdateScene(dt);
		draw();

		//uint drawFlags = PhysicsEngine::Instance()->GetDebugDrawFlags();

	}

private:
	std::vector<std::vector<GameObject*>> objects;

};