const path = require('path')
const fs = require('fs')
const semver = require('semver')
const { GitProcess } = require('dugite')
const { promisify } = require('util')

const { ELECTRON_DIR } = require('../lib/utils')

const readFile = promisify(fs.readFile)

const preType = {
  NONE: 'none',
  PARTIAL: 'partial',
  FULL: 'full'
}

const getCurrentDate = () => {
  const d = new Date()
  const dd = `${d.getDate()}`.padStart(2, '0')
  const mm = `${d.getMonth() + 1}`.padStart(2, '0')
  const yyyy = d.getFullYear()
  return `${yyyy}${mm}${dd}`
}

const isNightly = v => v.includes('nightly')
const isBeta = v => v.includes('beta')
const isStable = v => {
  const parsed = semver.parse(v)
  return !!(parsed && parsed.prerelease.length === 0)
}

const makeVersion = (components, delim, pre = preType.NONE) => {
  let version = [components.major, components.minor, components.patch].join(delim)
  if (pre === preType.PARTIAL) {
    version += `${delim}${components.pre[1] || 0}`
  } else if (pre === preType.FULL) {
    version += `-${components.pre[0]}${delim}${components.pre[1]}`
  }
  return version
}

async function nextBeta (v) {
  const next = semver.coerce(semver.clean(v))

  const tagBlob = await GitProcess.exec(['tag', '--list', '-l', `v${next}-beta.*`], ELECTRON_DIR)
  const tags = tagBlob.stdout.split('\n').filter(e => e !== '')
  tags.sort((t1, t2) => semver.gt(t1, t2))

  // increment the latest existing beta tag or start at beta.1 if it's a new beta line
  return tags.length === 0 ? `${next}-beta.1` : semver.inc(tags.pop(), 'prerelease')
}

async function getElectronVersion () {
  const versionPath = path.resolve(ELECTRON_DIR, 'ELECTRON_VERSION')
  const version = await readFile(versionPath, 'utf8')
  return version.trim()
}

async function nextNightly (v) {
  let next = semver.valid(semver.coerce(v))
  const pre = `nightly.${getCurrentDate()}`

  const branch = (await GitProcess.exec(['rev-parse', '--abbrev-ref', 'HEAD'], ELECTRON_DIR)).stdout.trim()
  if (branch === 'master') {
    next = semver.inc(await getLastMajorForMaster(), 'major')
  } else if (isStable(v)) {
    next = semver.inc(next, 'patch')
  }

  return `${next}-${pre}`
}

async function getLastMajorForMaster () {
  let branchNames
  const result = await GitProcess.exec(['branch', '-a', '--remote', '--list', 'origin/[0-9]-[0-9]-x'], ELECTRON_DIR)
  if (result.exitCode === 0) {
    branchNames = result.stdout.trim().split('\n')
    const filtered = branchNames.map(b => b.replace('origin/', ''))
    return getNextReleaseBranch(filtered)
  } else {
    throw new Error('Release branches could not be fetched.')
  }
}

function getNextReleaseBranch (branches) {
  const converted = branches.map(b => b.replace(/-/g, '.').replace('x', '0'))
  return converted.reduce((v1, v2) => semver.gt(v1, v2) ? v1 : v2)
}

module.exports = {
  isStable,
  isBeta,
  isNightly,
  nextBeta,
  makeVersion,
  getElectronVersion,
  nextNightly,
  preType
}
