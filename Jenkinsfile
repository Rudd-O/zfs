// https://github.com/Rudd-O/shared-jenkins-libraries
@Library('shared-jenkins-libraries@master') _
pipeline {

    agent none

    options {
        checkoutToSubdirectory 'src/zfs'
        disableConcurrentBuilds()
    }

    triggers {
        pollSCM('H * * * *')
    }

    parameters {
        string defaultValue: '23 25 26 27', description: '', name: 'RELEASE', trim: true
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
                        script: "cd src/zfs && git rev-parse --short HEAD",
                        returnStdout: true
                    ).trim()
                    println "Git hash is reported as ${env.GIT_HASH}"
                }
                sh '''
                    cp -a "$JENKINS_HOME"/userContent/mocklock .
                    cp -a "$JENKINS_HOME"/shell_lib.sh .
                '''
                sh "rm -rf build dist"
                stash includes: 'mocklock', name: 'mocklock'
                stash includes: 'shell_lib.sh', name: 'shell_lib'
                stash includes: 'src/**', name: 'src'
            }
        }
        stage('Parallelize') {
            agent { label 'master' }
            steps {
                script {
                    def axisList = [
                        params.RELEASE.split(' '),
                    ]
                    def task = {
                        def myRelease = it[0]
                        return {
                            node('zfs') {
                                stage("Setup ${it.join(' ')}") {
                                    unstash 'mocklock'
                                    unstash 'shell_lib'
                                    sh "rm -rf build dist"
                                    sh "./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --no-clean --no-cleanup-after --install kernel-devel zlib-devel libuuid-devel libblkid-devel libattr-devel openssl-devel"
                                    sh """
                                        # make sure none of these unpleasant things are installed in the chroot prior to building
                                        output=\$(/usr/local/bin/mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --shell 'rpm -q libuutil1 libzpool2 libzfs2-devel zfs libzfs2' | grep -v '^package ' || true)
                                        if [ "\$output" != "" ] ; then
                                            ./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --remove libuutil1 libzpool2 libzfs2-devel zfs libzfs2
                                        fi
                                    """
                                    sh "./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --unpriv --shell 'mkdir -p /builddir/zfs && rm -rf /builddir/zfs/zfs /builddir/zfs/zfs-builtrpms'"
                                }
                                stage("Copy source ${it.join(' ')}") {
                                    unstash 'src'
                                    sh """
                                        find src/zfs -xtype l -print0 | xargs -0 -n 1 -i bash -c 'test -f "\$1" || { rm -f "\$1" && touch "\$1" ; }' -- {}
                                        # Copy ZFS source.
                                        ./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --copyin src/zfs/ /builddir/zfs/zfs/
                                        # Ensure that copied files are owned by mockbuild, not by root.
                                        ./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --shell 'cd /builddir/zfs && chown mockbuild -R zfs'
                                    """
                                }
                                stage("Build SRPMs ${it.join(' ')}") {
                                    script {
                                        def program = """
                                            ./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --unpriv --shell '
                                                set -e -x -o pipefail
                                                mkdir -p /builddir/zfs/zfs-builtrpms
                                                (
                                                    cd /builddir/zfs/zfs
                                                    ./autogen.sh
                                                    sed "s/_META_RELEASE=.*/_META_RELEASE=0.${env.BUILD_NUMBER}.${env.GIT_HASH}/" -i configure
                                                    ./configure --with-config=user
                                                    make srpm-dkms srpm-utils
                                                    mv *.rpm ../zfs-builtrpms
                                                ) 2>&1 | sed -u "s/^/zfs: /" &
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
                                stage("Copy SRPMs out ${it.join(' ')}") {
                                    sh """
                                        ./mocklock -r fedora-${myRelease}-${env.BRANCH_NAME}-x86_64-generic --copyout /builddir/zfs/zfs-builtrpms/ build/
                                    """
                                }
                                stage("Build RPMs ${it.join(' ')}") {
                                    sh """
                                        . ./shell_lib.sh
                                        mockfedorarpms "${myRelease}" "dist/RELEASE=${myRelease}" build/*.src.rpm
                                    """
                                }
                                stage("Stash ${it.join(' ')}") {
                                    sh "find dist/ | sort"
                                    stash includes: "dist/RELEASE={$myRelease}/**", name: "dist-${myRelease}"
                                }
                            }
                        }
                    }
                    parallel funcs.combo(task, axisList)
                }
            }
        }
        stage('Collect') {
            agent { label 'master' }
            steps {
                script {
                    for (r in params.RELEASE.split(' ')) {
                        unstash "dist-${r}"
                    }
                }
                sh "find dist/ | sort"
                archiveArtifacts 'dist/**'
                fingerprint 'dist/**'
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
