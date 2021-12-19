#pragma once
#include "config.hpp"
#include "geometry.hpp"

struct Command {
	// Next should calculate and save a delta in an private property in
	// order to to Apply it after.
	virtual bool Next (void) = 0;
	virtual void Apply (void) = 0;
	virtual void Halt (void) = 0;
	virtual void End (void) { Halt(); }
	virtual ~Command (void) = default;
};

struct Move : public Command {
	unitid_t uid;
	struct point target;
	struct point mv;
	angle_t a;

	bool Next (void) override;
	void Apply (void) override;
	void Halt (void) override;
	Move (void) = default;
	Move (unitid_t uid, struct point tar);
};

struct Attack : public Command {
	unitid_t uid;
	unitid_t target;
	unsigned int lastatktime;
	bool outofrange;
	Move mv;

	bool Next (void) override;
	void Apply (void) override;
	void Halt (void) override;
	Attack (void) = default;
	Attack (unitid_t uid, unitid_t target);
};
