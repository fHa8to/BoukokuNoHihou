#pragma once
#include "DxLib.h"
#include "Game.h"
#include <memory>
#include <cmath>

#define PLAYER_MAX_HITCOLL 21872 //��������R���W�����|���S���̍ő吔

// �ő�g�o
#define PLAYER_HP_MAX		50
// �g�o�P�łǂꂾ���o�[��L�΂���
#define PLAYER_DRAW_SIZE	10


class Enemy;
class Stage;

class Player
{
public:
	Player();
	virtual ~Player();

	void Load();
	void Delete();

	void Init();
	void Update(Stage& stage);
	void Draw();

	//�J�����̕������擾
	void SetCameraAngle(float angle) { m_cameraAngle = angle; }

	//�v���C���[�̍��W���擾
	const VECTOR& GetPos() const { return m_pos; }
	void SetPos(const VECTOR pos) { m_pos = pos; }

	//�v���C���[��HP���擾
	const int& GetHp() const { return m_hp; }
	void SetHp(const int hp) { m_hp = hp; }


	//�U���̎擾
	const bool& GetUnderAttack() const { return m_isUnderAttack; }


	void SetIsDown(const bool isDeath) { m_isDeath = isDeath; }

	//�����蔻��̔��a
	float GetRadius() { return m_modelRadius; }


	//�J�v�Z���̓����蔻��
	bool IsCapsuleColliding(std::shared_ptr<Enemy> m_pEnemy);

	//�U���̓����蔻��
	bool IsAttackColliding(std::shared_ptr<Enemy> m_pEnemy);

	//���ލU���̓����蔻��
	bool IsStepOnAttockColliding(std::shared_ptr<Enemy> m_pEnemy);

	void CorrectPosition(Stage& stage);

	int SetColor(int color) { return m_color = color; }


	enum State
	{
		kIdle,		//�ҋ@
		kWalk,		//����
		kRun,		//�_�b�V��
		kJump,		//�W�����v
		kAttack,	//�U��
		kFall,		//������
		kDamage,	//�_���[�W
		kDeath		//��

	};

private:
	//�A�j���[�V�����̐i�s
	//���[�v�������ǂ�����Ԃ�
	bool UpdateAnim(int attachNo);
	void ChangeAnim(int animIndex);


	/*�t���O*/
	State m_nowState;
	State m_backState;
	State isGetState();


	/*����*/
	//�ړ�
	void Move();
	//�W�����v
	void Jump();


	/*�A�j���[�V����*/
	//�ҋ@
	void IdleAnim();
	//����
	void WalkAnim();
	//�_�b�V��
	void RunAnim();
	//�W�����v
	void JumpAnim();
	//������
	void FallAnim();
	//�U��
	void AttackAnim();
	//�_���[�W
	void DamageAnim();
	//��
	void DeathAnim();



private:
	//���f���n���h��
	int m_modelHandle;

	//�A�j���[�V�������
	int m_animIndex;
	int m_currentAnimNo;	//���݂̃A�j���[�V����
	int m_prevAnimNo;		//�ύX�O�̃A�j���[�V����
	float m_animBlendRate;	//�A�j���[�V������������

	float m_angle;

	float m_modelRadius;
	float m_radius;
	float m_attackRadius;

	float m_JumpPower;

	//�J�������
	float m_cameraAngle;

	//�\�����
	VECTOR m_pos;
	VECTOR m_attackPos;
	VECTOR m_attackDir;
	VECTOR m_move;
	VECTOR m_enemyPos;
	VECTOR m_mapHitColl;    //�L�����N�^�[�̃}�b�v�Ƃ̓����蔻��


	//�J�v�Z���̓_
	VECTOR m_topA;
	VECTOR m_bottomA;
	VECTOR m_topB;
	VECTOR m_bottomB;

	//�ړ�
	VECTOR move;
	float m_analogX;
	float m_analogZ;

	//HP
	int m_hp;
	float m_speed;  //�L�����̃X�s�[�h


	//state�t���O
	bool m_isMove;
	bool m_isAttack;	//�U��
	bool m_isWalk;		//����
	bool m_isRun;		//�_�b�V��
	bool m_isJump;		//�W�����v
	bool m_isIdle;		//�ҋ@
	bool m_isFloor;		//������
	bool m_isDamag;		//�_���[�W
	bool m_isDeath;		//��

	//�A�j���[�V�����t���O
	bool m_isStopEnd;	// �A�j���[�V�����Ō�Œ�~������
	int m_runFrame;


	int m_color = 0xffffff;

	//�����蔻��̔����t���O
	bool m_isUnderAttack;


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