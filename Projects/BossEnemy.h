#pragma once
#include "DxLib.h"
#include "Game.h"
#include <math.h>
#include <memory>
#include <unordered_map>

//#define PLAYER_MAX_HITCOLL 21836 //��������R���W�����|���S���̍ő吔

//// �ő�g�o
//#define BOSS_ENEMY_HP_MAX		16
//// �g�o�P�łǂꂾ���o�[��L�΂���
//#define BOSS_ENEMY_DRAW_SIZE	51


class Player;
class Stage;
class Ui;

class BossEnemy
{
public:
	BossEnemy();
	virtual ~BossEnemy();
	void Init();
	void Update(std::shared_ptr<Player> m_pPlayer, std::shared_ptr<Ui> m_pUi, Stage& stage);
	void Draw(std::shared_ptr<Player> m_pPlayer);
	void End();


	//�G�l�~�[�̍��W���擾
	VECTOR& GetPos() { return m_pos; }
	void SetPos(const VECTOR pos) { m_pos = pos; }


	float GetRadius() { return m_modelRadius; }

	//�U���̎擾
	const bool& GetUnderAttack() const { return m_isUnderAttack; }

	int SetColor(int color) { return m_color = color; }

	enum State
	{
		kIdle,		//�ҋ@
		kRun,		//����
		kAttack,	//�U��
		kDamage,	//�_���[�W
		kDeath,		//��
	};

	State GetState() { return m_state; }
	void SetState(State state) { m_state = state; }

	bool Translation(std::shared_ptr<Player> m_pPlayer);

	//�U���̓����蔻��
	bool IsAttackColliding(std::shared_ptr<Player> m_pPlayer);

	bool IsStopColliding(std::shared_ptr<Player> m_pPlayer);

	void CorrectPosition(Stage& stage);

	//�_���[�W�̃t���O�擾
	void SetDamage(const bool damage) { m_isDamage = damage; }


private:
	//�A�j���[�V�����̐i�s
//���[�v�������ǂ�����Ԃ�
	bool UpdateAnim(int attachNo);
	void ChangeAnim(int animIndex);

	std::unordered_map<State, float> m_animSpeedMap; // �e�X�e�[�g�ɑΉ�����A�j���[�V�����̍Đ����x


private:
	//���f���n���h��
	int m_modelHandle;
	int m_handle;


	//�A�j���[�V�������
	int m_animIndex;
	int m_currentAnimNo;	//���݂̃A�j���[�V����
	int m_prevAnimNo;		//�ύX�O�̃A�j���[�V����
	float m_animBlendRate;	//�A�j���[�V������������
	float m_animSpeed;		//�A�j���[�V�������x


	//�\�����
	VECTOR m_pos;
	VECTOR m_attackPos;
	VECTOR m_angle;
	VECTOR m_move;
	VECTOR m_playerPos;
	VECTOR m_direction;
	VECTOR m_mapHitColl;    //�L�����N�^�[�̃}�b�v�Ƃ̓����蔻��


	//�i�ދ���
	VECTOR m_distance;

	//�����蔻��̔��a
	float m_modelRadius;
	float m_discoveryRadius;
	float m_attackRadius;
	float m_stopRadius;


	int TargetNumber;


	//�A�j���[�V�����t���O
	int m_isIdle;
	int m_isAttack;
	int m_isRnu;
	int m_isDamage;
	int m_isDead;


	//State�ϐ�
	State m_state;

	//�����蔻��̔����t���O
	bool m_isUnderAttack;

	// �U���̒x�����Ǘ�����ϐ�
	int m_attackDelayCounter;

	int m_color = 0xffffff;

	//HP
	int m_hp;


	bool m_moveFlag;
	bool m_hitFlag;
	MV1_COLL_RESULT_POLY_DIM HitDim;
	int m_kabeNum;
	int m_yukaNum;
	MV1_COLL_RESULT_POLY* Kabe[PLAYER_MAX_HITCOLL];	// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* Yuka[PLAYER_MAX_HITCOLL];	// ���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* Poly;				// �|���S���̍\���̂ɃA�N�Z�X���邽�߂Ɏg�p����|�C���^( �g��Ȃ��Ă��ς܂����܂����v���O�����������Ȃ�̂ŁE�E�E )
	HITRESULT_LINE LineRes;				// �����ƃ|���S���Ƃ̓����蔻��̌��ʂ�������\����
	VECTOR OldPos;						// �ړ��O�̍��W	
	VECTOR NowPos;						// �ړ���̍��W};

};

