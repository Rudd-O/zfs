// https://github.com/Rudd-O/shared-jenkins-libraries
@Library('shared-jenkins-libraries@master') _


def srpm_step() {
    return {
        dir('src') {
            script {
                env.GIT_HASH = sh (
                    script: "git rev-parse --short HEAD",
                    returnStdout: true
                ).trim()
                println "Git hash is reported as ${env.GIT_HASH}"
                sh """
                ./autogen.sh
                sed "s/_META_RELEASE=.*/_META_RELEASE=0.${env.BUILD_NUMBER}.${env.GIT_HASH}/" -i configure
                ./configure --with-config=user
                make srpm-dkms srpm-utils
                """
            }
        }
    }
}

def integration_step() {
    return {
        build job: "zfs-fedora-installer/master", parameters: [[$class: 'StringParameterValue', name: 'UPSTREAM_PROJECT', value: "zfs/${currentBuild.projectName}"], [$class: 'StringParameterValue', name: 'BUILD_FROM_RPMS', value: "yes"], [$class: 'StringParameterValue', name: 'BUILD_FROM_SOURCE', value: "no"], [$class: 'StringParameterValue', name: 'SOURCE_BRANCH', value: "master"], [$class: 'StringParameterValue', name: 'RELEASE', value: "${params.RELEASE}"]]
    }
}

genericFedoraRPMPipeline(
    null,
    srpm_step(),
    ['autoconf', 'automake', 'libtool', 'zlib-devel', 'libuuid-devel', 'libtirpc-devel', 'libblkid-devel', 'openssl-devel'],
    integration_step(),
)
