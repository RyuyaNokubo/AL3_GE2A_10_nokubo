#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;
const float PI = 3.141592;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
}

void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("mario.jpg");
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	model_ = Model::Create();

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲(回転角用)
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲(座標用)
	std::uniform_real_distribution<float> posDist(-10.f, 10.f);

	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		angle[i] = (i + 1) * XM_2PI / 10;
		// X,Y,Z 方向のスケーリングを設定
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		// X,Y,Z 軸回りの回転角を設定
		worldTransform_[i].rotation_ = {0, 0, 0};
		// X,Y,Z 軸回りの平行移動を設定
		worldTransform_[i].translation_ = {10.0f * cosf(angle[i]), 10.0f * sinf(angle[i]), 0};

		//ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}

	//ビュープロジェクション
	viewProjection_.Initialize();

	//スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	//音声再生
	// audio_->PlayWave(soundDataHandle_);
	// voiceHandle_ = audio_->PlayWave(soundDataHandle_, true);
}

void GameScene::Update() {
	//速さ
	const float speed = 0.03f;

	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		//移動ベクトル
		angle[i] += speed;
		angle[i] = fmodf(angle[i], XM_2PI);
		worldTransform_[i].translation_.x = 10.0f*(cosf(angle[i]) - sinf(angle[i]));
		worldTransform_[i].translation_.y = 10.0f*(sinf(angle[i]) + cosf(angle[i]));

		worldTransform_[i].UpdateMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dモデル描画
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	/*sprite_->Draw();*/

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
