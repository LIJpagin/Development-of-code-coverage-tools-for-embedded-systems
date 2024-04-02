/*
 *  CoverageLogger.h
 *  Created on: 18 окт. 2023 г.
 *      Author: Shpagin_D
 */

#ifndef COVERAGE_LOGGER_H
#define COVERAGE_LOGGER_H

#include "base/Interfaces.h"

#if defined(USING_LOGGING) && defined(USING_LOGGING_COVERAGE)

#define GET_COV_LOGGER() Logging::CoverageLogger::GetInstance()
#define COV_LOG(statement, numberFile)	{ Logging::CovEntry entry(statement, numberFile, \
				__LINE__); GET_COV_LOGGER()->Log(entry); }
#define COV_LOG_IF(numberFile)		COV_LOG(Logging::Statement::If, numberFile)
#define COV_LOG_ELSE(numberFile)	COV_LOG(Logging::Statement::Else, numberFile)
#define COV_LOG_FOR(numberFile)		COV_LOG(Logging::Statement::For, numberFile)
#define COV_LOG_WHILE(numberFile)	COV_LOG(Logging::Statement::While, numberFile)
#define COV_LOG_DO(numberFile)		COV_LOG(Logging::Statement::DoWhile, numberFile)
#define COV_LOG_CASE(numberFile)	COV_LOG(Logging::Statement::SwitchCase, numberFile)
#define COV_LOG_FUNC(numberFile)	COV_LOG(Logging::Statement::Function, numberFile)
#define COV_LOG_LAMBDA(numberFile)	COV_LOG(Logging::Statement::Lambda, numberFile)

namespace Logging
{


class CoverageLogger :
	public ILogger,
	public IUsingSink<CovEntry, SIZE_OF_ENTRIES_IN_COV_SINK>,
	public Singleton<CoverageLogger>
{
	friend class Singleton;
	
	unsigned _numberOfLostEntries { 0 };

	void Log(const Level level, const char* format, ...) override;
	void Log(LogEntry& entry) override;

public:
	CoverageLogger(const CoverageLogger&) = delete;
	CoverageLogger(CoverageLogger&& moved) = delete;
	const CoverageLogger& operator=(const CoverageLogger&) = delete;
	CoverageLogger& operator=(CoverageLogger&& moved) = delete;

	/// Получить количество потерянных записей
	unsigned GetNumberOfLostEntries() const;
	void Log(CovEntry& entry);

	void Dump() override;

protected:
	CoverageLogger() = default;
	~CoverageLogger() override = default;
};


} // namespace Logging

#else

// Пустые макросы в случае отсутствия логирования покрытия
#define GET_COV_LOGGER() nullptr
#define COV_LOG(...)
#define COV_LOG_IF()
#define COV_LOG_ELSE()
#define COV_LOG_FOR()
#define COV_LOG_WHILE()
#define COV_LOG_DO()
#define COV_LOG_CASE()
#define COV_LOG_FUNC()
#define COV_LOG_LAMBDA()

#endif // defined(USING_LOGGING) && defined(USING_LOGGING_COVERAGE)
#endif // COVERAGE_LOGGER_H
