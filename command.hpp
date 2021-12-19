#pragma once
#include "config.hpp"
#include "geometry.hpp"

class Command {
public:
	// Next should calculate and save a delta in an private property in
	// order to to Apply it after.
	virtual bool Next (void) = 0;
	virtual void Apply (void) = 0;
	virtual void Halt (void) = 0;
	virtual void End (void) { Halt(); }
	virtual ~Command (void) = default;
};

class Move : public Command {
	unitid_t uid;
	struct point target;
	struct point mv;
	angle_t a;
public:
	bool Next (void) override;
	void Apply (void) override;
	void Halt (void) override;
	Move (unitid_t uid, struct point tar);
};

class Attack : public Command {
	unitid_t uid;
	unitid_t target;
	struct point mv;
	unsigned int lastatktime;
public:
	bool Next (void) override;
	void Apply (void) override;
	void Halt (void) override;
	Attack (unitid_t uid, unitid_t target);
};
