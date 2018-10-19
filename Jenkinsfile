// https://github.com/Rudd-O/shared-jenkins-libraries
@Library('shared-jenkins-libraries@master') _

def RELEASE = funcs.loadParameter('parameters.groovy', 'RELEASE', '28')

pipeline {

    agent none

    options {
        disableConcurrentBuilds()
    }

    triggers {
        pollSCM('* * * * *')
    }

    parameters {
        string defaultValue: '', description: "Override which Fedora releases to build for.  If empty, defaults to ${RELEASE}.", name: 'RELEASE', trim: true
    }

    stages {
        stage('Preparation') {
            agent { label 'master' }
            steps {
                script {
                    funcs.announceBeginning()
                }
                script {
                    env.GIT_HASH = sh (
                        script: "git rev-parse --short HEAD",
                        returnStdout: true
                    ).trim()
                    println "Git hash is reported as ${env.GIT_HASH}"
                }
                sh "git clean -fxd && rm -rf src/zfs"
                sh '''
                    cp -a "$JENKINS_HOME"/shell_lib.sh "$JENKINS_HOME"/userContent/mocklock .
                '''
                stash includes: '**', name: 'src'
            }
        }
        stage('Parallelize') {
            agent { label 'master' }
            steps {
                script {
                    if (params.RELEASE != '') {
                        RELEASE = params.RELEASE
                    }
                    def axisList = [
                        RELEASE.split(' '),
                    ]
                    def task = {
                        def myRelease = it[0]
                        return {
                            node('zfs') {
                                stage("Setup ${it.join(' ')}") {
                                    timeout(time: 15, unit: 'MINUTES') {
                                        sh "./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic -v --install glibc-devel libtirpc-devel kernel-devel zlib-devel libuuid-devel libblkid-devel libattr-devel openssl-devel"
                                        sh """
                                            # make sure none of these unpleasant things are installed in the chroot prior to building
                                            output=\$(/usr/local/bin/mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --shell 'rpm -q libuutil1 libzpool2 libzfs2-devel zfs libzfs2' | grep -v '^package ' || true)
                                            if [ "\$output" != "" ] ; then
                                                ./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --remove libuutil1 libzpool2 libzfs2-devel zfs libzfs2
                                            fi
                                        """
                                        sh "./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --unpriv --shell 'mkdir -p /builddir/zfs && rm -rf /builddir/zfs/zfs /builddir/zfs/zfs-builtrpms'"
                                    }
                                }
                                stage("Copy source ${it.join(' ')}") {
                                    timeout(time: 5, unit: 'MINUTES') {
                                        sh "rm -rf * .git"
                                        unstash "src"
                                        sh """
                                            # Copy ZFS source.
                                            ./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --copyin . /builddir/zfs/zfs/
                                            # Ensure that copied files are owned by mockbuild, not by root.
                                            ./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --shell 'cd /builddir/zfs && chown mockbuild -R zfs'
                                        """
                                    }
                                }
                                stage("Build SRPMs ${it.join(' ')}") {
                                    timeout(time: 15, unit: 'MINUTES') {
                                        script {
                                            def program = """
                                                ./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --unpriv --shell '
                                                    set -e -x -o pipefail
                                                    mkdir -p /builddir/zfs/zfs-builtrpms
                                                    cd /builddir/zfs/zfs
                                                    ./autogen.sh
                                                    sed "s/_META_RELEASE=.*/_META_RELEASE=0.${env.BUILD_NUMBER}.${env.GIT_HASH}/" -i configure
                                                    ./configure --with-config=user
                                                    make srpm-dkms srpm-utils
                                                    mv *.rpm ../zfs-builtrpms
                                                    zfspid=\$!
                                                    wait \$zfspid || retval=\$?
                                                    exit \$retval
                                                '
                                            """
                                            println "Program to be run:"
                                            println program
                                            sh program
                                        }
                                    }
                                }
                                stage("Copy SRPMs out ${it.join(' ')}") {
                                    timeout(time: 5, unit: 'MINUTES') {
                                        sh """
                                            ./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --copyout /builddir/zfs/zfs-builtrpms/ build/
                                        """
                                    }
                                }
                                stage("Build RPMs ${it.join(' ')}") {
                                    timeout(time: 20, unit: 'MINUTES') {
                                        sh """
                                            . ./shell_lib.sh
                                            mockfedorarpms "${myRelease}" "dist/RELEASE=${myRelease}" build/*.src.rpm
                                        """
                                    }
                                }
                                stage("Archive ${it.join(' ')}") {
                                    timeout(time: 5, unit: 'MINUTES') {
                                        archiveArtifacts artifacts: 'dist/**', fingerprint: true
                                    }
                                }
                            }
                        }
                    }
                    parallel funcs.combo(task, axisList)
                }
            }
        }
        stage('Publish') {
            agent { label 'master' }
            steps {
                copyArtifacts(
                        projectName: JOB_NAME,
                        selector: specific(BUILD_NUMBER),
                )
                script {
                    if (env.BRANCH_NAME == "master") {
                        funcs.uploadDeliverables('dist/*/*.rpm')
                    }
                }
            }
        }
    }
    post {
        always {
            node('master') {
                script {
                    funcs.announceEnd(currentBuild.currentResult)
                }
            }
        }
    }
}
