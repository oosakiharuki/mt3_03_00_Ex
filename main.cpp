#include <Novice.h>
#include"MyMath.h"
#include <cstdint>
#include <cassert>
#include<imgui.h>

const char kWindowTitle[] = "LE2C_07_オオサキ_ハルキ_タイトル";



Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

Vector3 Bezier(const Vector3& p0, const Vector3& p1, float t) {

	Vector3 a = { t * p0.x ,t * p0.y ,t * p0.z };
	Vector3 b = { (1.0f - t) * p1.x,(1.0f - t) * p1.y,(1.0f - t) * p1.z };
	Vector3	c = { a.x + b.x,a.y + b.y,a.z + b.z };

	return c;
}

Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {

	Vector3 a = { ((-p0.x + (3 * p1.x) - (3 * p2.x) + p3.x) * (t * t * t) + ((2 * p0.x) - (5 * p1.x) + (4 * p2.x) - p3.x) * (t * t) + ((-p0.x + p2.x) * t) + (2 * p1.x)) / 2 ,
					((-p0.y + (3 * p1.y) - (3 * p2.y) + p3.y) * (t * t * t) + ((2 * p0.y) - (5 * p1.y) + (4 * p2.y) - p3.y) * (t * t) + ((-p0.y + p2.y) * t) + (2 * p1.y)) / 2,
					((-p0.z + (3 * p1.z) - (3 * p2.z) + p3.z)* (t* t* t) + ((2 * p0.z) - (5 * p1.z) + (4 * p2.z) - p3.z) * (t * t) + ((-p0.z + p2.z) * t) + (2 * p1.z)) / 2  };

	return	a;
}


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHandleWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHandleWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHandleWidth + (xIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ x,0.0f,-kGridHandleWidth };
		Vector3 end{ x,0.0f,kGridHandleWidth };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (x == 0.0f) {
			color = BLACK;
		}
		Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, color);
	}
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHandleWidth + (zIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ -kGridHandleWidth,0.0f,z };
		Vector3 end{ kGridHandleWidth,0.0f,z };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (z == 0.0f) {
			color = BLACK;
		}
		Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, color);

	}
}

void DrawCatmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {

	uint32_t link = 16;
	uint32_t index = 0;

	Vector3 p0Screen = Transform(Transform(controlPoint0, viewProjectionMatrix), viewportMatrix);
	Vector3 p1Screen = Transform(Transform(controlPoint1, viewProjectionMatrix), viewportMatrix);
	Vector3 p2Screen = Transform(Transform(controlPoint2, viewProjectionMatrix), viewportMatrix);
	Vector3 p3Screen = Transform(Transform(controlPoint3, viewProjectionMatrix), viewportMatrix);

	Vector3 aScreen = Transform(Transform(controlPoint0, viewProjectionMatrix), viewportMatrix);

	Novice::DrawEllipse((int)aScreen.x, (int)aScreen.y, 5, 5, 0.0f, BLACK, kFillModeSolid);
	Novice::DrawEllipse((int)p1Screen.x, (int)p1Screen.y, 5, 5, 0.0f, BLACK, kFillModeSolid);
	Novice::DrawEllipse((int)p2Screen.x, (int)p2Screen.y, 5, 5, 0.0f, BLACK, kFillModeSolid);
	Novice::DrawEllipse((int)p3Screen.x, (int)p3Screen.y, 5, 5, 0.0f, BLACK, kFillModeSolid);
		
	Vector3 a = p1Screen;

		
	Vector3 b = p2Screen;

		
	Vector3 c = p3Screen;

		
	Vector3 CatmullRom0 = {};	
	Vector3 CatmullRom1 = {};		
	Vector3 CatmullRom2 = {};		
	Vector3 CatmullRom3 = {};

	for (index = 0; index < link; index++) {
		float t0 = index / float(link);
		float t1 = index / float(link);
		float t2 = index / float(link);
		float t3 = index / float(link);



		CatmullRom0 = CatmullRom(p0Screen, p0Screen, p1Screen, p2Screen, t0);
		CatmullRom1 = CatmullRom(p0Screen, p1Screen, p2Screen, p3Screen, t1);
		CatmullRom2 = CatmullRom(p1Screen, p2Screen, p3Screen, p3Screen, t2);
		CatmullRom3 = CatmullRom(p1Screen, p2Screen, p3Screen, p3Screen, t3);


		if (index == 0)
		{
			Novice::DrawLine((int)p1Screen.x, (int)p1Screen.y, (int)a.x, (int)a.y, color);
			Novice::DrawLine((int)p2Screen.x, (int)p2Screen.y, (int)b.x, (int)b.y, color);
			Novice::DrawLine((int)p3Screen.x, (int)p3Screen.y, (int)c.x, (int)c.y, color);
		}
		else {

			Novice::DrawLine((int)a.x, (int)a.y, (int)CatmullRom0.x, (int)CatmullRom0.y, color);
			Novice::DrawLine((int)b.x, (int)b.y, (int)CatmullRom1.x, (int)CatmullRom1.y, color);
			Novice::DrawLine((int)c.x, (int)c.y, (int)CatmullRom2.x, (int)CatmullRom2.y, color);

		}
		a.x = CatmullRom0.x;
		a.y = CatmullRom0.y;
		a.z = CatmullRom0.z;

		b.x = CatmullRom1.x;
		b.y = CatmullRom1.y;
		b.z = CatmullRom1.z;

		c.x = CatmullRom2.x;
		c.y = CatmullRom2.y;
		c.z = CatmullRom2.z;

		//float t0 = index / float(link);
		//float t1 = index / float(link);

		//Vector3 bezier0 = Bezier(p0Screen, p1Screen, t0);
		//Vector3 bezier1 = Bezier(p1Screen, p2Screen, t1);
		//Vector3 p = Bezier(bezier0, bezier1, t0);

		//if (index == 0) {
		//	Novice::DrawLine((int)p0Screen.x, (int)p0Screen.y, (int)a.x, (int)a.y, RED);
		//}
		//else {
		//	Novice::DrawLine((int)a.x, (int)a.y, (int)p.x, (int)p.y, color);
		//}
		//a.x = p.x;
		//a.y = p.y;
	}
}




// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 controlPoints[4]{
		{-0.8f,0.58f,1.0f},
		{1.76f,1.0f,-0.3f},
		{0.94f,-0.7f,2.3f},
		{-0.53f,-0.26f,-0.15f}
	};

	Vector3 cameraPosition = { 0.0f ,0.0f,-20.0f };
	Vector3 cameraTranslate = { 0.0f,-1.0f,-6.49f };
	Vector3 cameraRotate = { -0.26f,0.26f,0.0f };

	MyMath* myMath_ = new MyMath();

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		

		Matrix4x4 worldMatrix = myMath_->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 cameraMatrix = myMath_->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, cameraPosition);
		Matrix4x4 viewMatrix = myMath_->Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = myMath_->MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 100.0f);
		Matrix4x4 WorldViewProjectionMatrix = myMath_->Multiply(worldMatrix, myMath_->Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = myMath_->MakeViewportMatrix(0, 0, float(1280.0f), float(720.0f), 0.0f, 1.0f);


		DrawCatmullRom(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3], WorldViewProjectionMatrix, viewportMatrix, BLUE);

		DrawGrid(WorldViewProjectionMatrix, viewportMatrix);

		ImGui::Begin("window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);

		ImGui::DragFloat3("p0", &controlPoints[0].x, 0.1f);
		ImGui::DragFloat3("p1", &controlPoints[1].x, 0.1f);
		ImGui::DragFloat3("p2", &controlPoints[2].x, 0.1f);
		ImGui::DragFloat3("p3", &controlPoints[3].x, 0.1f);

		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
